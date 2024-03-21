/*
 * Create custom RAC class, for example “AbortActionCustom”, which extends
 * com.teamcenter.rac.workflow.common.actions.AbortAction.
 */
    public AbortAction(AbstractAIFUIApplication theApplication, String theActionName)
    {
        super(theApplication, theActionName);
    }

    public AbortAction(AbstractAIFUIApplication theApplication, Registry theRegistry, String theActionName)
    {
        super(theApplication, theRegistry, theActionName);
}
/* 
 * In this custom class,  override the “run()” method to perform some extra validation.
 * If it fails, display the error message box.  If it passes, run “super.run()”.
 * User Overrides the property_user value of “abortAction” in rac by overriding
 * the value with the classpath of the custom RAC class, i.e.
 */
abortAction=com.teamcenter.rac.workflow.common.actions.AbortActionCustom
