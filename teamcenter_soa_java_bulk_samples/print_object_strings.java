/*
*
    The SOA Java HelloTeamcenter Query.java example 
    found in soa_client.zip was used to test this method.
*
*/
import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.exceptions.NotLoadedException;


    public void printObject_strings(ModelObject[] objects)
    {
        DataManagementService  dmService = DataManagementService.getService(AppXSession.getConnection());
        if (objects.length == 0) return;
        String[] attributes = {"object_string"};
        dmService.getProperties(objects, attributes);
        for (int i = 0; i < objects.length; i++)
        {
            String type = objects[i].getTypeObject().getName();
            String objectString = "";
            if (objects[i].getTypeObject().isInstanceOf("POM_object"))
            {
                try
                {
                    objectString = wo.getPropertyObject("object_string").getStringValue();
                }
                catch (NotLoadedException e) {} // just ignore
            }
            System.out.println("    " + objectString + "(" + type + ")" );
        }
    }
