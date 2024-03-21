/*
This SOA java example will demonstrate how to find an ItemRevision using 'item_id' and 'item_revision_id' property.

The HelloTeamcenter java example was used to test this example (found in, soa_client.zip)

Service, com.teamcenter.services.strong.query.SavedQueryService is used to define, 'queryService'
*/


public void businessObjectQuery2()
{
	BusinessObjectQueryInput input = new BusinessObjectQueryInput();
	BusinessObjectQueryClause clause = new BusinessObjectQueryClause();
	BusinessObjectQueryClause clause2 = new BusinessObjectQueryClause();
	
	clause.mathOperator = "=";
	clause.logicOperator = "";
	clause.propName = "items_tag.item_id";
	clause.propValue = "000104";
	
	clause2.mathOperator = "=";
	clause2.logicOperator = "AND";
	clause2.propName = "item_revision_id";
	clause2.propValue = "A";
	
	input.clientId = input.getClass().getName() + Integer.toString(input.hashCode());
	input.maxNumToReturn = 0;
	input.typeName = "ItemRevision";
	input.clauses = new BusinessObjectQueryClause[]{ clause, clause2 };
	
	SavedQueriesResponse resp = queryService.executeBusinessObjectQueries(new BusinessObjectQueryInput[]{ input });
	
	if(!ServiceDataError(resp.serviceData))
	{
		for(QueryResults result : resp.arrayOfResults)
		{
			for(String UID : result.objectUIDS)
			{
				System.out.println("UID: " + UID);
				ServiceData sData = dmService.loadObjects(result.objectUIDS);
									
				for(int i = 0; i < sData.sizeOfPlainObjects(); i++)
				{
					ModelObject obj = sData.getPlainObject(i);
					
					if(obj instanceof WorkspaceObject)
					{
						try
						{
							System.out.println(((WorkspaceObject)obj).get_object_name());
						}
						catch(NotLoadedException e)
						{
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				}
			}
		}
		
	}	
}

protected boolean ServiceDataError(final ServiceData data)
{
	if(data.sizeOfPartialErrors() > 0)
	{
		for(int i = 0; i < data.sizeOfPartialErrors(); i++)
		{
			for(String msg : data.getPartialError(i).getMessages())
				System.out.println(msg);
		}

		return true;
	}

	return false;
}