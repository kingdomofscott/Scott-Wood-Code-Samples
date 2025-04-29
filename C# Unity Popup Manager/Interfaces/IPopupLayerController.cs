namespace PopupManager
{
    //This interface is given to the PopupController to more directly affect the layer
    public interface IPopupLayerController
    {
        void Close(PopupUnloadType popupUnload, bool externalClose = false, bool unloadAndClose = true);
        void ClearViewLayer();
        void DeleteViewLayer();
        bool IsPopupLoaded();
        void AddPopupToUnload(List<PopupController> toUnload);
        void SetupNewPopupOperation(PopupController newCurrent);
        void ReplaceCurrentPopup(PopupController controller, IPopupRequest request, PopupUnloadType popupUnload);
        ILoadPopupOperationObserver InsertImmediatePopupAtFrontOfQueue(IPopupRequest request);
        void LoadFinished();
        void CloseLayer();
    }
}