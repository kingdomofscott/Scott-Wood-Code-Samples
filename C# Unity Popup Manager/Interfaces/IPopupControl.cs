namespace PopupManager
{
    //This interface is given to exterior components for basic functionality
    public interface IPopupLayerController
    {
        void Close(PopupUnloadType popupUnload);
        void ReplaceCurrentPopup(IPopupRequest request, PopupUnloadType popupUnload);
        void InsertNextPopup(IPopupRequest request, PopupUnloadType popupUnload);
        ILoadPopupOperationObserver OverlayPopup(IPopupRequest request);
        PopupIdentifier GetIdentifier();
    }
}