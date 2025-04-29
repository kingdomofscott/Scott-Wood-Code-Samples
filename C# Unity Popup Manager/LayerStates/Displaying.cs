namespace PopupManager.States
{
    public class Displaying : LayerState
    {
        public Displaying(PopupLayerController newParentStateMachine) : base(newParentStateMachine)
        {
        }

        public override void ManagedUpdate()
        {
            ParentStateMachine.ControllerList.ManagedUpdate();
        }

        public override void ManagedOnDestroy()
        {
            ParentStateMachine.ControllerList.ManagedOnDestroy();
        }
    }
}
