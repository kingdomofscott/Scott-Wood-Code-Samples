namespace PopupManager
{
    //This interface is given to interior components of the popup manager to more directly affect the queue and layers
    public interface IInternalPopupManager
    {
        ILoadPopupOperationObserver LoadOverlayPopup(IPopupRequest request);
        PopupController CheckForNextPopup();
        void AddPopupOnNewTopLayer(PopupController newCurrent);
        void RemovePopupLayer(IPopupLayerController layer);
        void RemoveAllPopupLayers();
        bool UnloadLayer(PopupUnloadType popupUnload, IPopupLayerController layer);
        void LayerStartingToClose(IPopupLayerController layer);
        void ReplaceCurrentPopup(IPopupLayerController layer, PopupController controller, IPopupRequest request, PopupUnloadType popupUnload);
    }
}