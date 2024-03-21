/*

Prerequisite: Teamcenter Security Services installed and correctly working. For example, AWC or RAC installation successfully using SSO for login.

'host' : Teamcenter URL.  (starts with tccs|http)
'ssoServerhost' : Security Services Identity Service URL.
'ssoAppID' : Security Services AppID.
'mySessionDiscriminator' : your unique session discrimintator. Please see SOA javadocs for more information.

This java code will support 2 or 4 tier connection. TCCS, 2 tier; HTTP/S, 4 tier.

Author: Patrick Hoonhout

*/

package com.teamcenter.hello;

import com.teamcenter.schemas.soa._2006_03.exceptions.InvalidCredentialsException;
import com.teamcenter.schemas.soa._2006_03.exceptions.InvalidUserException;
import com.teamcenter.schemas.soa._2006_03.exceptions.ServiceException;
import com.teamcenter.services.strong.core.SessionService;
import com.teamcenter.services.strong.core._2006_03.Session.LoginResponse;
import com.teamcenter.soa.SoaConstants;
import com.teamcenter.soa.client.Connection;
import com.teamcenter.soa.client.SsoCredentials;
import com.teamcenter.soa.client.model.ServiceData;
import com.teamcenter.soa.client.model.strong.User;
import com.teamcenter.soa.exceptions.CanceledOperationException;

public class SsoSession 
{
        private User user;
        
        private Connection connection;
        private SsoCredentials ssoCredentials;
        private SessionService sessionService;
        private final String ssoAppID = "Teamcenter";
        
        public SsoSession(String host, String ssoServerhost)
        {
               String protocol = (host.startsWith("http")) ? SoaConstants.HTTP : SoaConstants.TCCS;

               ssoCredentials = new SsoCredentials(ssoServerhost, ssoAppID);
               connection = new Connection(host, ssoCredentials, SoaConstants.REST, protocol);
               sessionService = SessionService.getService(connection); 
        }
        
        public boolean ssoLogin()
        {
               if(ssoCredentials != null && connection != null)
               {
                       try 
                       {
                              String[] credentials = ssoCredentials.getCredentials(new InvalidUserException());
                              LoginResponse resp = sessionService.loginSSO(credentials[0], credentials[1], credentials[2], credentials[3], credentials[4], "mySessionDiscriminator");
                              
                              if(!ServiceDataError(resp.serviceData))
                              {
                                      user = resp.user;
                                      return true;
                              }
                       } 
                       catch (InvalidCredentialsException | CanceledOperationException e) 
                       {
                              e.printStackTrace();
                       }
               }
               
               return false;
        }
        
        public Connection getConnection() { return connection; }
        
        public void logout()
        {
        try
        {
            sessionService.logout();
        }
        catch (ServiceException e){}
        }
        
        public User getUser() { return user; }
        
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
}
