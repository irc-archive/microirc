package main;

import java.io.IOException;
import java.util.Random;

import javax.persistence.EntityManager;
import javax.persistence.EntityManagerFactory;
import javax.persistence.Persistence;
import javax.persistence.PersistenceContext;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import model.User;

public class RunTest extends HttpServlet {
	    
	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
	    
//		Ejb3Configuration cfg = new Ejb3Configuration();
//		EntityManagerFactory emf = 
//		  cfg.addProperties( properties ) //add some properties
//		     .setInterceptor( myInterceptorImpl ) // set an interceptor
//		     .addAnnotatedClass( MyAnnotatedClass.class ) //add a class to be mapped
//		     .addClass( NonAnnotatedClass.class ) //add an hbm.xml file using the Hibernate convention
//		     .addRerousce( "mypath/MyOtherCLass.hbm.xml" ) //add an hbm.xml file
//		     .addRerousce( "mypath/orm.xml" ) //add an EJB3 deployment descriptor
//		     .configure("/mypath//hibernate.cfg.xml") //add a regular hibernate.cfg.xml
//		     .buildEntityManagerFactory(); //Create the entity manager factory
		
	    EntityManagerFactory emf = Persistence.createEntityManagerFactory("MyJPAcfg");
		EntityManager em = emf.createEntityManager();
				
        User user = new User();
        //user.setUserId(1);
        user.setDisplayName("Joe Smith");
        user.setEmail("a"+new Random().nextFloat());
        user.setPassword("b");

        // Persist the customer
        em.getTransaction().begin();
        em.persist(user);
        em.getTransaction().commit();
        
		resp.getOutputStream().write("asdasdasda".getBytes());
	}

}
