namespace PopupManager
{
    public class State : PopupManagerController.State
    {
        public State(PopupManagerController newParentStateMachine) : base(newParentStateMachine)
        {
        }

        public virtual void ManagedOnDestroy()
        {
        }

        public virtual void ManagedUpdate()
        {
        }
    }
}
