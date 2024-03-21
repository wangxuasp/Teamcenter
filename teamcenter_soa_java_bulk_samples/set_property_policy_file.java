public void setPolicyFile()
{
    String policy = connection.getCurrentObjectPropertyPolicy();
    System.out.println("\n Current Policy Name: " + policy);

    SessionService sessionSvc = SessionService.getService(connection);

    try 
    {
      System.out.println(" Setting Policy File...\n");

      // Policy File: %TC_DATA%\soa\policies\Test.xml
	boolean setPolicy = sessionSvc.setObjectPropertyPolicy("Test");			   
    } 
    catch (ServiceException e) 
    {			
        e.printStackTrace();
    }

    String newPolicy= connection.getCurrentObjectPropertyPolicy();
    System.out.println(" New Policy Name: " + newPolicy);
}