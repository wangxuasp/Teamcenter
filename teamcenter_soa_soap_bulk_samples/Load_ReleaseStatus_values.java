/* This example was tested with the java HelloTcSoap example provided in the soa_client.zip file.
It assumes you have the HelloTcSoap example working correctly. 

This example will demonstrate how to load the Fnd0ReleaseStatus object and iterate over its values.
 
Add the following XML code to the HelloTcSoap Build.xml file (after other codegenwsdlfilename nodes) 
to auto-generate the needed java stub classes.
-----------------------------------------------------------------------------------------------
 
      <codegenwsdlfilename="${soa.home}/wsdls/Core0806SessionService.wsdl"
               output="${axis.out}"serverSide="false"unpackClasses="true"/>
      <codegenwsdlfilename="${soa.home}/wsdls/Core0709DataManagementService.wsdl"
             output="${axis.out}"serverSide="false"unpackClasses="true"/>     
      <codegenwsdlfilename="${soa.home}/wsdls/Query0806SavedQueryService.wsdl"
                    output="${axis.out}"serverSide="false"unpackClasses="true"/>
      <codegenwsdlfilename="${soa.home}/wsdls/Query1004SavedQueryService.wsdl"
                    output="${axis.out}"serverSide="false"unpackClasses="true"/>
      <codegenwsdlfilename="${soa.home}/wsdls/Core1305LOVService.wsdl"
                    output="${axis.out}"serverSide="false"unpackClasses="true"/>
					
 
The java code uses the following PLMXML QueryBuilder definition to search for ListOfValues. 
You will need to import it using the RAC so the query definition is available for the java code:
 
XML Query definition for "__ListOfValues". Save as .xml file and use QueryBuilder to import.
--------------------------------------------------------------------------------------------
<?xml version="1.0" encoding="utf-8"?>
<!-- GENERATED BY: PLM XML SDK 7.0.3.296 -->
<plmxml_bus:PLMXMLBusinessTypesxmlns="http://www.plmxml.org/Schemas/PLMXMLSchema"
xmlns:plmxml_bus="http://www.plmxml.org/Schemas/PLMXMLBusinessSchema"
schemaVersion="6" language="en-us" date="2015-10-23" languages="en-usfr-fr de-de it-it es-es pt-br" time="11:49:39" author="Teamcenter V10000.1.0.42_20150727.00 -gtac@IMC--2023576316(-2023576316)">
<plmxml_bus:SavedQueryDef id="id1" nameRef="#id2" descriptionTextRef="#id3" name="__ListOfValues" queryFlag="0" queryClass="ListOfValues">
<Description>List LOVs</Description>
<ApplicationRef version="AzZZC6WE4Zi2AB" application="Teamcenter" label="AzZZC6WE4Zi2AB"></ApplicationRef>
<plmxml_bus:QueryClause id="id4" stringValue="SELECTqid FROM ListOfValues WHERE &quot;lov_name&quot; = &quot;${lov_name = }&quot; AND &quot;lov_type&quot; = &quot;${lov_type = }&quot; ORDER BY lov_name ASCENDING USERENTRY lov_name"></plmxml_bus:QueryClause></plmxml_bus:SavedQueryDef>
<plmxml_bus:Text id="id2" primary="en-us"></plmxml_bus:Text>
<plmxml_bus:Text id="id3" primary="en-us"></plmxml_bus:Text></plmxml_bus:PLMXMLBusinessTypes>
 
 
Add the following code the the Hello.java to call/use the custom java LovService:
---------------------------------------------------------------------------------
 
	LovService lov = new LovService();
       
	lov.listLOV("Fnd0ReleaseStatus","ListOfValuesStringExtentStatus");

	
Add the following class to package com.teamcenter.hellosoap.
------------------------------------------------------------ */
 
package com.teamcenter.hellosoap;
 
import java.math.BigInteger;
import java.rmi.RemoteException;
 
import com.teamcenter.schemas.core._2007_09.datamanagement.LoadObjectsInput;
import com.teamcenter.schemas.core._2008_06.session.SetObjectPropertyPolicyInput;
import com.teamcenter.schemas.core._2013_05.lov.GetInitialLOVValuesInput;
import com.teamcenter.schemas.core._2013_05.lov.InitialLovData;
import com.teamcenter.schemas.core._2013_05.lov.LOVSearchResults;
import com.teamcenter.schemas.core._2013_05.lov.LOVValueRow;
import com.teamcenter.schemas.core._2013_05.lov.LovFilterData;
import com.teamcenter.schemas.core._2013_05.lov.PropertyValues;
import com.teamcenter.schemas.query._2006_03.savedquery.GetSavedQueriesInput;
import com.teamcenter.schemas.query._2006_03.savedquery.GetSavedQueriesResponse;
import com.teamcenter.schemas.query._2006_03.savedquery.SavedQueryObject;
import com.teamcenter.schemas.query._2007_09.savedquery.SavedQueriesResponse;
import com.teamcenter.schemas.query._2008_06.savedquery.ExecuteSavedQueriesInput;
import com.teamcenter.schemas.query._2008_06.savedquery.QueryInput;
import com.teamcenter.schemas.query._2010_04.savedquery.FindSavedQueriesCriteriaInput;
import com.teamcenter.schemas.query._2010_04.savedquery.FindSavedQueriesInput;
import com.teamcenter.schemas.query._2010_04.savedquery.FindSavedQueriesResponse;
import com.teamcenter.schemas.soa._2006_03.base.ErrorStack;
import com.teamcenter.schemas.soa._2006_03.base.ErrorValue;
import com.teamcenter.schemas.soa._2006_03.base.ModelObject;
import com.teamcenter.schemas.soa._2006_03.base.ObjectPropertyPolicy_type0;
import com.teamcenter.schemas.soa._2006_03.base.PolicyProperty;
import com.teamcenter.schemas.soa._2006_03.base.PolicyType;
import com.teamcenter.schemas.soa._2006_03.base.Property;
import com.teamcenter.schemas.soa._2006_03.base.PropertyValue;
import com.teamcenter.schemas.soa._2006_03.base.ServiceData;
import com.teamcenter.services.core._2007_09.Core0709DataManagementService;
import com.teamcenter.services.core._2008_06.Core0806SessionService;
import com.teamcenter.services.core._2013_05.Core1305LOVService;
import com.teamcenter.services.core._2013_05.InternalServerFaultFault;
import com.teamcenter.services.core._2013_05.InvalidUserFaultFault;
import com.teamcenter.services.query._2006_03.Query0603SavedQueryService;
import com.teamcenter.services.query._2008_06.Query0806SavedQueryService;
import com.teamcenter.services.query._2010_04.Query1004SavedQueryService;
import com.teamcenter.services.query._2010_04.ServiceFaultFault;
 
 
public class LovService
{
      privatefinal StringsavedQuery ="__ListOfValues";
      
      private Core0709DataManagementServicedm0709Service;
      private Core1305LOVServicelov1305Service;
      private Query0603SavedQueryServiceqry0603Service;
      private Query0806SavedQueryServiceqry0806Service;
      private Query1004SavedQueryServiceqry1004Service;
      private ModelObjectdynamicLOV;
      private ModelObjectquery;
 
      public LovService()
       {     
             dm0709Service = (Core0709DataManagementService)Session.createServiceStub(Core0709DataManagementService.class);
             lov1305Service = (Core1305LOVService)Session.createServiceStub(Core1305LOVService.class);
             qry0603Service = (Query0603SavedQueryService)Session.createServiceStub(Query0603SavedQueryService.class);
             qry0806Service = (Query0806SavedQueryService)Session.createServiceStub(Query0806SavedQueryService.class);
             qry1004Service = (Query1004SavedQueryService)Session.createServiceStub(Query1004SavedQueryService.class);
             
              setObjectPolicy();
       }
      
      publicvoid listLOV(final String name, final String type)
       {
             if(setSavedQuery2())
              {
                    try
                     {
                           ExecuteSavedQueriesInput input =new ExecuteSavedQueriesInput();
                           QueryInput qryInput =new QueryInput();
                           String ID ="ID:" + qryInput.hashCode();
                          
                           qryInput.setClientId(ID);
                           qryInput.setRequestId(ID);
                           qryInput.setMaxNumToReturn(BigInteger.valueOf(0));
                           qryInput.setQuery(query);
                           qryInput.setResultsType(BigInteger.valueOf(0));
                           qryInput.setEntries(new String[]{ "lov_name","lov_type" });                                                                                                        
                           qryInput.setValues(new String[]{ name, type });
                    
                           input.setInput(new QueryInput[]{ qryInput });
 
                           SavedQueriesResponse qryResp =qry0806Service.executeSavedQueries(input);
                          
                          if(!ServiceDataError(qryResp.getServiceData()))
                           {
                                 if(qryResp.getArrayOfResults().length == 1)
                                  {
                                         LoadObjectsInput objInput =new LoadObjectsInput();
                                        
                                         objInput.setUids(qryResp.getArrayOfResults()[0].getObjectUIDS());
 
                                         ServiceData sData =dm0709Service.loadObjects(objInput);
                                        
                                        if(!ServiceDataError(sData))
                                         {
                                                ClientDataModel.addObjects(sData);
                                                ModelObject[] objs = ClientDataModel.getObjects(sData.getPlainObjs());
                                               
                                               for(ModelObject obj : objs)
                                                {
                                                       printListOfValues(obj);
                                                }
                                         }
                                  }
                           }     
                     }
                    catch (RemoteException e1)
                     {
                          // TODO Auto-generated catch block
                           e1.printStackTrace();
                     }
                    catch (com.teamcenter.services.query._2008_06.InternalServerFaultFault e1)
                     {
                          // TODO Auto-generated catch block
                           e1.printStackTrace();
                     }
                    catch (com.teamcenter.services.query._2008_06.InvalidUserFaultFault e1)
                     {
                          // TODO Auto-generated catch block
                           e1.printStackTrace();
                     }
                    catch (com.teamcenter.services.core._2007_09.InternalServerFaultFault e)
                     {
                          // TODO Auto-generated catch block
                           e.printStackTrace();
                     }
                    catch (com.teamcenter.services.core._2007_09.InvalidUserFaultFault e)
                     {
                          // TODO Auto-generated catch block
                           e.printStackTrace();
                     }
              }
       }
      
      protectedboolean setSavedQuery2()
       {
             try
              {     
                     FindSavedQueriesInput input =new FindSavedQueriesInput();
                     FindSavedQueriesCriteriaInput criteria =new FindSavedQueriesCriteriaInput();
                    
                     criteria.setQueryNames(new String[]{ savedQuery });
                     criteria.setQueryType(BigInteger.valueOf(0));
                     input.setInputCriteria(new FindSavedQueriesCriteriaInput[]{ criteria });
                    
                     FindSavedQueriesResponse resp =qry1004Service.findSavedQueries(input);
                    
                    if(!ServiceDataError(resp.getServiceData()))
                     {
                          if(resp.getSavedQueries().length == 1)
                           {
                                 query = resp.getSavedQueries()[0];
                                 returntrue;
                           }
                     }
              }
             catch (RemoteException e)
              {
                    // TODO Auto-generated catch block
                     e.printStackTrace();
              }
             catch (com.teamcenter.services.query._2010_04.InternalServerFaultFault e)
              {
                    // TODO Auto-generated catch block
                     e.printStackTrace();
              }
             catch (ServiceFaultFault e)
              {
                    // TODO Auto-generated catch block
                     e.printStackTrace();
              }
             catch (com.teamcenter.services.query._2010_04.InvalidUserFaultFault e)
              {
                    // TODO Auto-generated catch block
                     e.printStackTrace();
              }            
            
             return false;
       }
 
      protectedvoid printListOfValues(final ModelObject mObj)
       {
              Property[] properties = mObj.getProperties();
             
             if(properties !=null)
              {
                    for(Property prop : properties)
                     {
                           System.out.println("Property Name: " + prop.getName());
                           System.out.println("UI value: " + prop.getUiValue());
                           System.out.println("Values:");
                          
                          for(PropertyValue value : prop.getValues())
                           {
                                  System.out.println("\t" + value.getValue());
                           }
                          
                           System.out.println();
                     }
              }
       }
      
      protectedboolean ServiceDataError(final ServiceData data)
       {
             if (data.isPartialErrorsSpecified())
              {
                    for(ErrorStack error : data.getPartialErrors())
                     {
                          for (ErrorValue value : error.getErrorValues())
                                  System.out.println(value.getMessage());
                     }
 
                    returntrue;
              }
 
             return false;
       }
      
      protectedvoid setObjectPolicy()
       {
             try
              {
                     Core0806SessionService session = (Core0806SessionService)Session.createServiceStub(Core0806SessionService.class);
                     SetObjectPropertyPolicyInput input =new SetObjectPropertyPolicyInput();
                     ObjectPropertyPolicy_type0 policy =new ObjectPropertyPolicy_type0();
                     PolicyType ptype;
                     PolicyProperty property;
 
                     ptype =new PolicyType();
                     ptype.setName("ListOfValues");
                     property =new PolicyProperty();
                     property.setName("lov_name");
                     ptype.addProperties(property);
                     property =new PolicyProperty();
                     property.setName("lov_values");
                     ptype.addProperties(property);
                     policy.addTypes(ptype);
                    
                     input.setObjectPropertyPolicy(policy);
                     session.setObjectPropertyPolicy(input);
              }
             catch (RemoteException e)
              {
                     e.printStackTrace();
              }
             catch (com.teamcenter.services.core._2008_06.InternalServerFaultFault e)
              {
                     e.printStackTrace();
              }
             catch (com.teamcenter.services.core._2008_06.InvalidUserFaultFault e)
              {
                     e.printStackTrace();
              }
       }
}