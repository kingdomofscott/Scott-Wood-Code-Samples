namespace PopupManager.States
{
    public class DisplayingLayer : State
    {
        public DisplayingLayer(PopupManagerController newParentStateMachine) : base(newParentStateMachine)
        {
        }

        public override void ManagedUpdate()
        {
            if (!ParentStateMachine.IsDisplayingPopup())
            {
                ParentStateMachine.SwitchToIdle();
            }

            ParentStateMachine.ControllerList.ManagedUpdate();
        }

        public override void ManagedOnDestroy()
        {
            ParentStateMachine.ControllerList.ManagedOnDestroy();
        }
    }
}
