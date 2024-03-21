/*
*
    The SOA Java HelloTeamcenter Query.java example 
    found in soa_client.zip was used to test this method.
*
*/
import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;


    public ModelObject[] getModelObjects(String[] uids)
    {
    	ModelObject[] mObjs = null;
    	DataManagementService  dmService = 
			DataManagementService.getService(AppXSession.getConnection());
    	
		ServiceData sd = dmService.loadObjects(uids);
		mObjs = new ModelObject [sd.sizeOfPlainObjects()];
		for (int i = 0; i < sd.sizeOfPlainObjects(); ++i)
		{
			mObjs[i] = sd.getPlainObject(i);
		}    	
    	return mObjs;
    }
	
