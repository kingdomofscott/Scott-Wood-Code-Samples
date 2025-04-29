using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

namespace PopupManager
{
    public class PopupLayerController : StateMachine<PopupLayerController, LayerState>, IPopupLayerController
    {
        public ControllerList ControllerList { get; private set; }
        public IInternalPopupManager PopupManager { get; private set; }
        public PopupController CurrentPopup { get; private set; }
        public PopupIdentifier PopupIdentifier { get { return CurrentPopup.GetIdentifier(); } }

        private States.Closing _closingState;
        private States.Displaying _displayingState;
        private States.Loading _loadingState;

        private IInjectionContainer _injectionContainer;

        private Transform _layerTransform;
        private int _layerIndex;

        public PopupLayerController(IInternalPopupManager popupManager, PopupController popupOperation, int layerIndex)
        {
            ControllerList = new ControllerList();
            PopupManager = popupManager;
            CurrentPopup = popupOperation;
            _layerIndex = layerIndex;

            _loadingState = new States.Loading(this);
            _displayingState = new States.Displaying(this);
            _closingState = new States.Closing(this);
        }

        protected void Inject(IInjectionContainer parentInjectionContainer)
        {
            ControllerList.Inject(parentInjectionContainer);
            _injectionContainer = parentInjectionContainer;
        }

        protected void Initialize()
        {
            _layerTransform = PopupManager.View.GetNewLayer(_layerIndex, "Default");

            CurrentPopup.SetPopupLayer(this, ControllerList, _layerTransform);
            ControllerList.Initialize();
            SwitchTo(_loadingState);
        }

        public void ManagedUpdate()
        {
            CurrentState.ManagedUpdate();
        }

        public void ManagedOnDestroy()
        {
            CurrentState.ManagedOnDestroy();
        }

        //Layer Implementation
        public void Close(PopupUnloadType popupUnload, bool externalClose = false, bool unloadAndClose = true)
        {
            if(externalClose)
            {
                CurrentPopup.ExternallyClosed();
            }

            if (unloadAndClose)
            {
                _closingState.PopupUnload = popupUnload;
                SwitchTo(_closingState);
            }
        }

        public void ClearViewLayer()
        {
            PopupManager.View.ClearPopupLayer(_layerIndex);
        }

        public void DeleteViewLayer()
        {
            PopupManager.View.DeletePopupLayer(_layerIndex);
        }

        public bool IsPopupLoaded()
        {
            return CurrentPopup.IsLoaded();
        }

        public void AddPopupToUnload(List<PopupController> toUnload)
        {
            toUnload.Add(CurrentPopup);
        }

        public void SetupNewPopupOperation(PopupController newCurrent)
        {
            ControllerList.Clear();
            ControllerList.Inject(_injectionContainer);
            ControllerList.Initialize();

            CurrentPopup = newCurrent;
            CurrentPopup.SetPopupLayer(this, ControllerList, _layerTransform);

            SwitchTo(_loadingState);
            PopupManager.CurrentPopupLayerUpdated();
        }

        public void ReplaceCurrentPopup(PopupController controller, IPopupRequest request, PopupUnloadType popupUnload)
        {
            PopupManager.ReplaceCurrentPopup(this, controller, request, popupUnload);

            ControllerList.Clear();
            ControllerList.Inject(_injectionContainer);
            ControllerList.Initialize();

            SwitchTo(_loadingState);
        }

        public ILoadPopupOperationObserver InsertImmediatePopupAtFrontOfQueue(IPopupRequest request)
        {
            return PopupManager.InsertImmediatePopupAtFrontOfQueue(request);
        }

        public void LoadFinished()
        {
            SwitchTo(_displayingState);
        }

        public void CloseLayer()
        {
            PopupManager.RemovePopupLayer(this);
        }
    }
}
