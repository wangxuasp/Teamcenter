    public static void getAllItemRevisionsOfItem(ModelObject[] item_list)
    {
        if(item_list == null) return;

        SimpleDateFormat format = 
            new SimpleDateFormat("M/d/yyyy h:mm a", new Locale("en", "US"));
              
         for (int ii = 0; ii < item_list.length; ii++)
        {
            System.out.println("\nItem Revision List\tLast Modified");
            System.out.println("====================\t=============");
            
            DataManagementService dm = 
                DataManagementService.getService(Session.getConnection());
                
            String[] attributes = {"revision_list"};
            dm.getProperties(item_list, attributes);
            
            try
            {
                ModelObject[] rev_list = 
                    ((Item) item_list[ii]).get_displayable_revisions();
                for (int kk = 0; kk < rev_list.length; kk++)
                {
                    String objectString =
                        ((WorkspaceObject) rev_list[kk]).get_object_string();
                    Calendar lastModified = 
                        ((POM_application_object) rev_list[kk]).get_last_mod_date();
                    System.out.println(objectString + "\t\t" + 
                            format.format(lastModified.getTime()));
                }
            }
            catch (NotLoadedException e)
            {
                System.out.println(e.getMessage());
            }
        }
    }
