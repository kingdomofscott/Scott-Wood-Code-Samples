using System;
using System.Collections.Generic;
using UnityEngine;

namespace PopupManager
{
    public class PopupController : IPopupControl, ILoadPopupOperation
    {
        public IPopupRequest Request { get; set; }

        private Transform _popupCanvasTransform;
        private IInternalPopupManager _popupManager;
        private IPopupLayerController _popupLayer;
        private ControllerList _layerControllerList;
        private LoadAssetsOperation _assetLoadOperation;
        private OperationState _state;
        private Result _result;

        private IInjectionContainer _injectionContainer;
        private IAssetManager _assetService;

        private LoadAssetList _assetsToLoad;

        public QueuePopupOperation(IInternalPopupManager popupManager, IPopupRequest request, LoadAssetList assetsList)
        {
            _popupManager = popupManager;
            Request = request;
            _assetsToLoad = assetsList;
        }

        public void Inject(IInjectionContainer parentInjectionContainer)
        {
            _injectionContainer = parentInjectionContainer;
            _assetService = parentInjectionContainer.Resolve<IAssetManager>();

            Request.Inject(parentInjectionContainer);
        }

        public void Initialize()
        {
            Request.PopulateAssetsToLoad(_assetsToLoad);
        }

        public void SetPopupLayer(IPopupLayerController popupLayer, ControllerList controllerList, Transform popupTransform)
        {
            _popupLayer = popupLayer;
            _layerControllerList = controllerList;
            _popupCanvasTransform = popupTransform;
        }

        public void Close(PopupUnloadType popupUnload)
        {
            switch (_state)
            {
                case OperationState.done:
                    // Don't need to do anything here
                    break;
                case OperationState.running:
                    {
                        if (_assetLoadOperation != null)
                        {
                            _assetLoadOperation.Abort();
                        }

                        _result = Result.MakeValid();
                        _state = OperationState.done;
                        _popupLayer.Close(popupUnload);
                    }
                    break;
                case OperationState.waiting:
                    // Don't need to do anything here
                    break;
            }
        }

        public void ExternallyClosed()
        {
            switch (_state)
            {
                case OperationState.done:
                    // Don't need to do anything here
                    break;
                case OperationState.running:
                    {
                        if (_assetLoadOperation != null)
                        {
                            _assetLoadOperation.Abort();
                        }

                        _result = Result.MakeValid();
                        _state = OperationState.done;
                    }
                    break;
                case OperationState.waiting:
                    // Don't need to do anything here
                    break;
            }
        }

        public void ReplaceCurrentPopup(IPopupRequest request, PopupUnloadType popupUnload)
        {
            switch (_state)
            {
                case OperationState.done:
                    // Don't need to do anything here
                    break;
                case OperationState.running:
                    {
                        if (_assetLoadOperation != null)
                        {
                            _assetLoadOperation.Abort();
                        }

                        Reset();

                        _popupLayer.ReplaceCurrentPopup(this, request, popupUnload);

                        Request.Inject(_injectionContainer);
                        Initialize();
                    }
                    break;
                case OperationState.waiting:
                    // Don't need to do anything here
                    break;
            }
        }

        public void InsertNextPopup(IPopupRequest request, PopupUnloadType popupUnload)
        {
            switch (_state)
            {
                case OperationState.done:
                    // Don't need to do anything here
                    break;
                case OperationState.running:
                    {
                        if (_assetLoadOperation != null)
                        {
                            _assetLoadOperation.Abort();
                        }

                        _result = Result.MakeValid();
                        _state = OperationState.done;

                        _popupLayer.InsertImmediatePopupAtFrontOfQueue(request);
                        _popupLayer.Close(popupUnload);
                    }
                    break;
                case OperationState.waiting:
                    // Don't need to do anything here
                    break;
            }
        }

        public ILoadPopupOperationObserver OverlayPopup(IPopupRequest request)
        {
            return _popupManager.LoadOverlayPopup(request);
        }

        public PopupIdentifier GetIdentifier()
        {
            return Request.GetIdentifier();
        }

        public bool IsLoaded()
        {
            return _state == OperationState.done || (_state == OperationState.running && _assetLoadOperation == null);
        }

        public bool IsLoading()
        {
            return _state == OperationState.running && _assetLoadOperation != null && _assetLoadOperation.GetState() == OperationState.running;
        }

        public OperationState GetState()
        {
            return _state;
        }

        public OperationState Update()
        {
            if (_state == OperationState.waiting)
            {
                OnStart();

                _state = OperationState.running;
            }

            if (_state == OperationState.running)
            {
                if (_assetLoadOperation != null && _assetLoadOperation.GetState() == OperationState.done)
                {
                    var result = _assetLoadOperation.GetResult();

                    switch (result)
                    {
                        case error:
                            LoadOperationCancelledHandler(result.Error());
                            break;
                        case uninitialized:
                            LoadOperationCancelledHandler(new LoadOperationError(LoadErrorCode.Unknown, new List<LoadErrorInfo>()));
                            break;
                        case valid:
                            LoadOperationSuccessHandler();
                            break;
                    }
                }
            }

            return _state;
        }

        public void GetAllIdentifiers(List<PopupIdentifier> popupIds)
        {
            popupIds.Add(GetIdentifier());
        }

        public void GetAllPopupOperations(List<QueuePopupOperation> operations)
        {
            if (_state != OperationState.done)
            {
                operations.Add(this);
            }
        }

        public bool ReplaceDuplicates(Dictionary<PopupIdentifier, QueuePopupOperation> operations, List<QueuePopupOperation> immediateQueue)
        {
            var immediateIndex = immediateQueue.FindIndex(x => x.GetIdentifier() == this.GetIdentifier());

            if (immediateIndex != -1)
            {
                immediateQueue[immediateIndex].Abort();
                immediateQueue[immediateIndex] = this;
                return true;
            }
            else if (operations.ContainsKey(GetIdentifier()))
            {
                ReplaceDuplicateIdentifier(operations[GetIdentifier()]);
                return true;
            }
            else
            {
                operations.Add(GetIdentifier(), this);
            }

            return false;
        }

        public bool RemoveIdentifier(PopupIdentifier identifier, out int removed)
        {
            if (identifier == GetIdentifier())
            {
                Abort();
                return true;
            }

            return false;
        }

        public bool ReplaceDuplicateIdentifier(QueuePopupOperation operation)
        {
            if (operation.GetIdentifier() == GetIdentifier() && operation != this)
            {
                Abort();
                return true;
            }

            return false;
        }

        public float GetProgress()
        {
            switch (_state)
            {
                default:
                case waiting:
                    return 0;
                case running:
                    if (_assetLoadOperation != null)
                    {
                        return _assetLoadOperation.GetProgress();
                    }

                    return 1;
                case done:
                    return 1;
            }
        }

        public void CancelPendingPopupOperations()
        {
            if (_state != running)
            {
                Abort();
            }
        }

        public void Abort()
        {
            if (_state == running)
            {
                _state = done;
                _result = new Result();

                if (_assetLoadOperation != null)
                {
                    _assetLoadOperation.Abort();
                }

                _popupLayer.Close(PopupUnloadType.Unload);
            }

            if (_state != done)
            {
                _state = done;
                _result = new Result();
            }
        }

        public void Reset()
        {
            _state = waiting;
            _result = new Result();

            if (_assetLoadOperation != null)
            {
                _assetLoadOperation.Abort();
                _assetLoadOperation = null;
            }

            _assetsToLoad = new LoadAssetList();
        }

        public Result GetResult()
        {
            return _result;
        }

        public void Unload()
        {
            Request.Unload();
        }

        protected void OnStart()
        {
            _assetLoadOperation = _assetService.Load(_assetsToLoad);
        }

        private void LoadOperationSuccessHandler()
        {
            Result result;

            try
            {
                Request.Inject(_injectionContainer);
                result = LoadPopupAsset(_popupCanvasTransform, this);
            }
            catch (Exception exception)
            {
                LogHandledException(FeatureType.PopupManager, ErrorTypes.request_execute_exception);

                result = Result.MakeError();
            }

            switch (result)
            {
                case error:
                    {
                        LogError(FeatureType.PopupManager, ErrorTypes.error, "PopupExecuteError." + Request.GetIdentifier());
                        _result = Result.MakeError(new Exception("Unable to instantiate " + Request));
                        _state = OperationState.done;
                    }
                    break;
                case uninitialized:
                    {
                        LogError(FeatureType.PopupManager, ErrorTypes.unitialized, "PopupExecuteAborted." + Request.GetIdentifier());
                        Abort();
                    }
                    break;
                case valid:
                    break;
            }

            _assetLoadOperation = null;
        }

        private void LoadOperationCancelledHandler(LoadOperationError error)
        {
                LogError(FeatureType.PopupManager, ErrorTypes.cancel, "PopupLoadCancelError." + Request.GetIdentifier() + ": " + error);
                _result = Result.MakeError(new Exception(error.ToString()));
                _state = OperationState.done;
        }
    }
}
