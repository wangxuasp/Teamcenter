/*
*
    The SOA Java HelloTeamcenter Query.java example found in soa_client.zip was used to test this method.
*
*/

import com.teamcenter.services.strong.query.SavedQueryService;
import com.teamcenter.services.strong.query._2006_03.SavedQuery.GetSavedQueriesResponse;
import com.teamcenter.services.strong.query._2007_09.SavedQuery.SavedQueriesResponse;
import com.teamcenter.services.strong.query._2008_06.SavedQuery.QueryInput;
import com.teamcenter.services.strong.query._2007_09.SavedQuery.QueryResults;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.ImanQuery;

import com.teamcenter.soa.client.model.strong.Group;


    public Group findGroup(String groupName)
    {
        ImanQuery query = null;
    	Group group = null; 	

        SavedQueryService queryService = SavedQueryService.getService(AppXSession.getConnection());
        DataManagementService dmService= DataManagementService.getService(AppXSession.getConnection());
        try
        {
            GetSavedQueriesResponse savedQueries = queryService.getSavedQueries();
            if (savedQueries.queries.length == 0)
            {
                System.out.println("There are no saved queries in the system.");
                return group;
            }
			
            for (int i = 0; i < savedQueries.queries.length; i++)
            {

            	if (savedQueries.queries[i].name.equals("__WEB_group"))
                {
                    query = savedQueries.queries[i].query;
                    break;
                }
            }
        }
        catch (ServiceException e)
        {
            System.out.println("GetSavedQueries service request failed.");
            System.out.println(e.getMessage());
            return group;
        }

        if (query == null)
        {
            System.out.println("There is not an '__WEB_group' query.");
            return group;
        }

        try
        {
            QueryInput savedQueryInput[] = new QueryInput[1];
            savedQueryInput[0] = new QueryInput();
            savedQueryInput[0].query = query;
			savedQueryInput[0].entries = new String[1];
            savedQueryInput[0].values = new String[1];
            savedQueryInput[0].entries[0] = "Name" ;
            savedQueryInput[0].values[0] = groupName;
            savedQueryInput[0].maxNumToReturn = 25;
            
            SavedQueriesResponse savedQueryResult = queryService.executeSavedQueries(savedQueryInput);
            QueryResults found = savedQueryResult.arrayOfResults[0];

            int length = found.objectUIDS.length;
            
            String[] uids = new String[length];
            for( int ii = 0; ii < length; ii++)
            {
            	uids[ii]= found.objectUIDS[ii];
            }
            ServiceData sd = dmService.loadObjects(uids);
            ModelObject[] foundObjs = new ModelObject[ sd.sizeOfPlainObjects()];
            for( int k =0; k< sd.sizeOfPlainObjects(); k++)
            {
                foundObjs[k] = sd.getPlainObject(k);
            }
            group = (Group) foundObjs[0];   
        }
        catch (Exception e)
        {
            System.out.println("ExecuteSavedQuery service request failed.");
            System.out.println(e.getMessage());
        }            
        return group;
    }

