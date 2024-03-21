/*
*
    The SOA Java HelloTeamcenter project found in 
    soa_client.zip was used to test this method.
	
	This sample calls:
		Sample SOA Java method : print object strings (Document ID: tc_api593)
		Sample SOA Java method : get ModelObjects given UIDs (Document ID: tc_api596)
*
*/

import com.teamcenter.clientx.AppXSession;
import com.teamcenter.services.strong.core.DataManagementService;
import com.teamcenter.services.strong.core.ProjectLevelSecurityService;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.ModelObject;
import com.teamcenter.soa.client.model.strong.TC_Project;
import com.teamcenter.services.strong.core._2012_09.ProjectLevelSecurity.ProjectClientId;
import com.teamcenter.services.strong.core._2012_09.ProjectLevelSecurity.ProjectTeamsResponse;
import com.teamcenter.services.strong.core._2012_09.ProjectLevelSecurity.ProjectTeamsResponse;


    public void listMembers(TC_Project project)
    {
        ModelObject[] modelObjs = null;
        
        ProjectLevelSecurityService prjService = 
			ProjectLevelSecurityService.getService(AppXSession.getConnection());
        
        ProjectClientId projectclientid = new ProjectClientId();
        projectclientid.clientId= projectclientid.toString();
        projectclientid.tcProject=project;
        
        ProjectTeamsResponse ptRsp = 
			prjService.getProjectTeams(new ProjectClientId[]{projectclientid});

		int teamLen = ptRsp.projectTeams.length;			
		int regLen = ptRsp.projectTeams[0].regularMembers.length;
		int privLen = ptRsp.projectTeams[0].privMembers.length;
		int adminLen = ptRsp.projectTeams[0].projectTeamAdmins.length;

		
        System.out.println("\n\n Total Team Length: " + teamLen);
        for (int i = 0; i < teamLen; i++)
        {       
            if(regLen > 0 )
            {
                System.out.println("\n Regular Members (" + regLen + "):\n");
                String[] uids = new String[regLen];
                for(int j = 0; j < regLen; j++)
                {
                    uids[j] = ptRsp.projectTeams[i].regularMembers[j].getUid();
                }
                modelObjs = getModelObjects(uids);
                printObject_strings(modelObjs);
            }
            System.out.println();
            
            if(privLen > 0) 
            {
                System.out.println("\n Privileged Users (" + privLen + "):\n");
                String[] uids = new String[privLen];
                for(int j = 0; j < ptRsp.projectTeams[i].privMembers.length; j++)
                {
                    uids[j] = ptRsp.projectTeams[i].privMembers[j].getUid();
                }
                modelObjs = getModelObjects(uids);
                printObject_strings(modelObjs);
            }
            System.out.println();
                
            System.out.println("\n Team Adminstrators (" + adminLen + "):\n");
            String[] uids = new String[adminLen];
            if(adminLen > 0) 
            {
                for(int j = 0; j < adminLen; j++)
                {
                    uids[j] = ptRsp.projectTeams[i].projectTeamAdmins[j].getUid();
                }
                modelObjs = getModelObjects(uids);
                printObject_strings(modelObjs);
            }
        }
        System.out.println();   
    }