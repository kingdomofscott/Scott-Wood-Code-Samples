using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using System;

namespace PopupManager
{
    public class PopupManagerController : StateMachine<PopupManagerController, State>, IPopupManagerController, IInternalPopupManager
    {
        public ControllerList ControllerList { get; private set; }
        private List<IPopupLayerController> _popupLayers;
        public IPopupManagerView View { get; private set; }
        public int PopupLayerCount { get { return _popupLayers.Count; } }

        private List<PopupController> _immediatePopupQueue;
        private readonly Dictionary<PopupIdentifier, List<IBackgroundDownload>> _downloadObservers = new Dictionary<PopupIdentifier, List<IBackgroundDownload>>();

        private IInjectionContainer _injectionContainer;
        private readonly List<ILockHandle> _lockHandles;
        private IAssetManager _assetManager;

        private States.Idle _idleState;
        private States.DisplayingLayer _displayingLayerState;
        private States.Unloading _unloadingState;

        public PopupManagerController(IPopupManagerView view, IPopupFactory factory)
        {
            View = view;

            _lockHandles = new List<ILockHandle>();

            ControllerList = new ControllerList();

            _popupLayers = new List<IPopupLayerController>();
            _immediatePopupQueue = new List<PopupController>();

            _idleState = new States.Idle(this);
            _displayingLayerState = new States.DisplayingLayer(this);
            _unloadingState = new States.Unloading(this);
        }

        public void Update()
        {
            //Prune download list based on what has finished
            foreach (var download in _downloadObservers)
            {
                download.Value.RemoveAll((IBackgroundDownload observer) => observer.GetState() == OperationState.done);
            }

            CurrentState.ManagedUpdate();
        }

        public void ManagedOnDestroy()
        {
            CurrentState.ManagedOnDestroy();
        }

        protected void Initialize()
        {
            ControllerList.Initialize();

            SwitchTo(_idleState);
        }

        protected void Inject(IInjectionContainer injectionContainer)
        {
            _assetManager = injectionContainer.Resolve<IAssetManager>();
            _injectionContainer = injectionContainer;

            ControllerList.Inject(injectionContainer);
        }

        // IPopupManagerController Implementation
        public ILoadPopupOperationObserver QueueImmediatePopup(IPopupRequest request)
        {
            LoadAssetList assets = new LoadAssetList();
            var controller = new PopupController(this, request, null, assets, _analytics);

            controller.Inject(_injectionContainer);
            controller.Initialize();

            foreach (IPopupLayerController layer in _popupLayers)
            {
                if (layer.PopupIdentifier == controller.GetIdentifier())
                {
                    controller.Abort();

                    return new PopupController(controller);
                }
            }

            //Check for exisitng popups already queued
            var immediateIndex = _immediatePopupQueue.FindIndex(x => x.GetIdentifier() == controller.GetIdentifier());

            if (immediateIndex == -1)
            {
                _immediatePopupQueue.Add(controller);
            }
            else
            {
                _immediatePopupQueue[immediateIndex].Abort();
                _immediatePopupQueue[immediateIndex] = controller;
            }

            _immediatePopupQueue = _immediatePopupQueue.OrderBy(x => (int)x.GetIdentifier().Type).ToList();

            if (!IsLocked())
            {
                // If there is a current popup, use its property. Otherwise check immediate queue and use the property of its first request
                if (!IsDisplayingPopup() && _immediatePopupQueue.Count > 0)
                {
                    View.EnableBlocker(_immediatePopupQueue[0].Request.DisplayBlockerTint);
                }
            }

            AddToBackgroundDownloads(controller.Request.GetIdentifier(), assets, Priority.Low);

            return new PopupController(controller);
        }

        public ILoadPopupOperationObserver LoadOverlayPopup(IPopupRequest request)//This should only be called by a popup controller
        {
            LoadAssetList assets = new LoadAssetList();
            var controller = new PopupController(this, request, null, assets, _analytics);

            controller.Inject(_injectionContainer);
            controller.Initialize();

            if (GetTopPopupLayer().PopupIdentifier == controller.GetIdentifier())
            {
                controller.Abort();

                return new PopupController(controller);
            }

            var immediateIndex = _immediatePopupQueue.FindIndex(x => x.GetIdentifier() == controller.GetIdentifier());

            if (immediateIndex != -1)
            {
                _immediatePopupQueue[immediateIndex].Abort();
                _immediatePopupQueue.RemoveAt(immediateIndex);
                _immediatePopupQueue = _immediatePopupQueue.OrderBy(x => (int)x.GetIdentifier().Type).ToList();
            }

            DetermineBackgroundDownloadPriority(false);

            AddPopupOnNewTopLayer(controller, false);

            return new PopupController(controller);
        }

        public void RemoveFromQueue(PopupIdentifier identifier)
        {
            foreach (var controller in _immediatePopupQueue)
            {
                if (controller.GetIdentifier() == identifier)
                {
                    controller.Abort();
                }
            }

            _immediatePopupQueue.RemoveAll(controller => controller.GetIdentifier() == identifier);

            AbortBackgroundDownloads(identifier);

            int layersToRemove = 0;

            for (int i = _popupLayers.Count - 1; i >= 0; --i)
            {
                if (_popupLayers[i].PopupIdentifier == identifier)
                {
                    layersToRemove = _popupLayers.Count - i;
                    break;
                }
            }

            for (int i = 0; i < layersToRemove; ++i)
            {
                _popupLayers[i].Close(PopupUnloadType.Unload);
            }
        }

        public void ClearQueue()
        {
            RemoveAllPopupLayers();

            foreach (var controller in _immediatePopupQueue)
            {
                controller.Abort();
            }

            foreach (var downloadList in _downloadObservers)
            {
                foreach (var download in downloadList.Value)
                {
                    download.Abort();
                }
            }

            _immediatePopupQueue.Clear();
            _downloadObservers.Clear();
        }

        public ILockHandle Lock()
        {
            if (QueueCount() > 0 && CurrentState == _idleState)
            {
                View.DisableBlocker();
            }

            ILockHandle lockHandle = new LockHandle(this);
            _lockHandles.Add(lockHandle);

            return lockHandle;
        }

        public void Unlock(ILockHandle lockHandle)
        {
            _lockHandles.Remove(lockHandle);

            if (!IsLocked())
            {
                if (QueueCount() > 0)
                {
                    bool showTint = false;

                    if (IsDisplayingPopup())
                    {
                        showTint = GetTopPopupLayer().DisplayBlockerTint;
                    }
                    else if (_immediatePopupQueue.Count > 0)
                    {
                        showTint = _immediatePopupQueue[0].Request.DisplayBlockerTint;
                    }
                    // No immediate popups, but we know a popup is in the surface point queue because QueueCount > 0
                    else if (_immediatePopupQueue.Count == 0)
                    {
                        showTint = true;
                    }

                    View.EnableBlocker(showTint);
                }
            }
        }

        public bool IsLocked()
        {
            return _lockHandles.Count > 0;
        }

        public bool IsDisplayingPopup()
        {
            return _popupLayers.Count > 0;
        }

        public bool IsLoadingPopup()
        {
            return _immediatePopupQueue.Count > 0 && _immediatePopupQueue[0].IsLoading();
        }

        public int QueueCount()
        {
            return _immediatePopupQueue.Count + _popupLayers.Count;
        }

        public PopupIdentifier GetCurrentPopupIdentifier()
        {
            if (IsDisplayingPopup())
            {
                return GetTopPopupLayer().PopupIdentifier;
            }

            return PopupIdentifier.MakeError();
        }

        public ILoadPopupOperationObserver InsertImmediatePopupAtFrontOfQueue(IPopupRequest request)
        {
            LoadAssetList assets = new LoadAssetList();
            var controller = new PopupController(this, request, null, assets, _analytics);

            controller.Inject(_injectionContainer);
            controller.Initialize();

            var immediateIndex = _immediatePopupQueue.FindIndex(x => x.GetIdentifier() == controller.GetIdentifier());

            if (immediateIndex == -1)
            {
                _immediatePopupQueue.Insert(0, controller);
            }
            else
            {
                _immediatePopupQueue[immediateIndex].Abort();
                _immediatePopupQueue[immediateIndex] = controller;
            }

            AddToBackgroundDownloads(controller.Request.GetIdentifier(), assets, Priority.High);

            return new PopupController(controller);
        }

        public PopupController CheckForNextPopup()
        {
            if (IsLocked())
            {
                return null;
            }

            if (_immediatePopupQueue.Count > 0)
            {
                PopupController nextPopup = _immediatePopupQueue[0];
                _immediatePopupQueue.RemoveAt(0);

                return nextPopup;
            }

            return null;
        }

        public void AddPopupOnNewTopLayer(PopupController newCurrent)
        {
            PopupLayerController newLayer = new PopupLayerController(this, newCurrent, _popupLayers.Count);

            DetermineBackgroundDownloadPriority();

            ControllerList.Add(newLayer);
            _popupLayers.Add(newLayer);

            newCurrent.Update();
            SwitchToDisplaying();
        }

        public void RemovePopupLayer(IPopupLayerController layer)
        {
            DoRemoveLayer(layer);

            if (!IsDisplayingPopup())
            {
                SwitchToIdle();
            }
        }

        public void RemoveAllPopupLayers()
        {
            if (_popupLayers.Count > 0)
            {
                _popupLayers[0].Close(PopupUnloadType.Unload, true);
            }
        }

        public bool UnloadLayer(PopupUnloadType popupUnload, IPopupLayerController layer)
        {
            DoUnloadPopup(popupUnload, layer);

            if (IsUnloadingUnusedAssets())
            {
                DoRemoveLayer(layer);
                SwitchTo(_unloadingState);
                return true;
            }
            else
            {
                int layerIndex = GetLayerIndex(layer);

                for (int i = _popupLayers.Count - 1; i >= layerIndex + 1; --i)
                {
                    DoRemoveLayer(_popupLayers[i]);
                }

                return false;
            }
        }

        private void DoRemoveLayer(IPopupLayerController layer)
        {
            int layerIndex = GetLayerIndex(layer);

            for (int i = _popupLayers.Count - 1; i >= layerIndex; --i)
            {
                _popupLayers[i].DeleteViewLayer();
                ControllerList.Remove(_popupLayers[i]);
                _popupLayers.RemoveAt(i);
            }
        }

        public void LayerStartingToClose(IPopupLayerController layer)//Notify upper layers
        {
            int layerIndex = GetLayerIndex(layer);

            for (int i = layerIndex + 1; i < _popupLayers.Count; ++i)
            {//Unload will be handled by the base layer unload
                _popupLayers[i].Close(PopupUnloadType.DoNotUnload, true, false);
            }
        }

        //If multiple requests come in we need to prioritize and only show one popup chain at a time
        private void DetermineBackgroundDownloadPriority()
        {
            if (!_records.ClientConfigData.BackgroundDownloadConfig.PopupQueue)
            {
                return;
            }

            var nextType = new PopupIdentifier(PopupType.none);
            var afterType = new PopupIdentifier(PopupType.none);

            List<PopupController> copyImmediateQueue = _immediatePopupQueue;

            for (int i = 0; i < copyImmediateQueue.Count; ++i)
            {
                if (nextType.Type == PopupType.none)
                {
                    nextType = copyImmediateQueue[i].GetIdentifier();
                }
                else
                {
                    afterType = copyImmediateQueue[i].GetIdentifier();
                    ReprioritizeBackgroundDownloads(nextType, afterType);

                    return;
                }
            }

            ReprioritizeBackgroundDownloads(nextType, afterType);
        }

        private void ReprioritizeBackgroundDownloads(PopupIdentifier next, PopupIdentifier after)
        {
            if (next.Type != PopupType.none && _downloadObservers.ContainsKey(next))
            {
                for (int i = 0; i < _downloadObservers[next].Count; ++i)
                {
                    _downloadObservers[next][i].Reprioritize(Priority.High);
                }
            }

            if (after.Type != PopupType.none && _downloadObservers.ContainsKey(after))
            {
                for (int i = 0; i < _downloadObservers[after].Count; ++i)
                {
                    _downloadObservers[after][i].Reprioritize(Priority.Medium);
                }
            }
        }

        public void SwitchToIdle()
        {
            if (CurrentState != _idleState)
            {
                SwitchTo(_idleState);
            }
        }

        public void SwitchToDisplaying()
        {
            if (CurrentState != _displayingLayerState)
            {
                SwitchTo(_displayingLayerState);
            }
        }

        public void ReplaceCurrentPopup(IPopupLayerController layer, PopupController controller, IPopupRequest request, PopupUnloadType popupUnload)
        {
            DoUnloadPopup(popupUnload, layer);

            int layerIndex = GetLayerIndex(layer);

            for (int i = _popupLayers.Count - 1; i >= layerIndex + 1; --i)//Close all layers on top of this one
            {
                DoRemoveLayer(_popupLayers[i]);
            }

            controller.Request = request;

            var immediateIndex = _immediatePopupQueue.FindIndex(x => x.GetIdentifier() == controller.GetIdentifier());

            if (immediateIndex != -1)
            {
                _immediatePopupQueue[immediateIndex].Abort();
                _immediatePopupQueue[immediateIndex] = controller;
            }
        }

        private void DoUnloadPopup(PopupUnloadType popupUnload, IPopupLayerController layer)
        {
            int layerIndex = GetLayerIndex(layer);

            for (int i = _popupLayers.Count - 1; i >= layerIndex; --i)
            {
                _popupLayers[i].ClearViewLayer();
            }

            for (int i = _popupLayers.Count - 1; i >= layerIndex; --i)
            {
                if (popupUnload == PopupUnloadType.Unload || popupUnload == PopupUnloadType.DeferUnload)
                {
                    _popupLayers[i].UnloadPopup();
                }
            }

            if (popupUnload == PopupUnloadType.Unload)
            {
                _assetManager.UnloadUnusedAssets();
            }
        }

        private void AddToBackgroundDownloads(PopupIdentifier requestType, LoadAssetList assets, Priority downloadPriority)
        {
            List<string> popupAssets = assets.GetAssetNameList();
            List<IBackgroundDownload> downloads = new List<IBackgroundDownload>();

            foreach (var asset in popupAssets)
            {
                downloads.Add(_assetManager.QueueDownload(asset, downloadPriority, true));
            }

            if (!_downloadObservers.ContainsKey(requestType))
            {
                _downloadObservers[requestType] = downloads;
            }
            else
            {
                _downloadObservers[requestType].AddRange(downloads);
            }
        }

        private void AbortBackgroundDownloads(PopupIdentifier id)
        {
            if (_downloadObservers.ContainsKey(id))
            {
                for (int i = _downloadObservers[id].Count - 1; i >= 0; --i)
                {
                    _downloadObservers[id][i].Abort();
                }

                _downloadObservers[id].Clear();
            }
        }

        private IPopupLayerController GetTopPopupLayer()
        {
            if (_popupLayers.Count > 0)
            {
                return _popupLayers[_popupLayers.Count - 1];
            }

            return null;
        }

        private int GetLayerIndex(IPopupLayerController layer)
        {
            return _popupLayers.FindIndex(otherLayer => (otherLayer == layer));
        }
    }
}