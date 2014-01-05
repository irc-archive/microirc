package code;

import java.io.File;

import model.UserPersonalized;

import org.hibernate.Session;
import org.hibernate.SessionFactory;
import org.hibernate.Transaction;
import org.hibernate.cfg.Configuration;
import org.hibernate.tool.hbm2ddl.SchemaExport;
import org.hibernate.tool.hbm2ddl.SchemaUpdate;
import org.hibernate.tool.hbm2ddl.SchemaValidator;

public class MyTest {
	public static void main(String[] args) {
		SessionFactory sessionFactory = new Configuration().configure().buildSessionFactory();
		Session session = sessionFactory.getCurrentSession();
		Transaction tx = session.beginTransaction();
		UserPersonalized lecturer1 = new UserPersonalized();
		lecturer1.setFirstName("Fatma");
		lecturer1.setLastName("Meawad");
		session.save(lecturer1);
		tx.commit();
		System.out.println("The lecturer " + lecturer1.getFirstName() + " "
				+ lecturer1.getLastName()
				+ " is successfully added to your database");
		
//		The same of ant.xml ||| Hibernate tools to generate hbm from database
//		Configuration cfg = new Configuration();
//		new SchemaExport(cfg).create(false, true);
//		new SchemaUpdate(cfg).execute(false, true);
//		new SchemaValidator(cfg).validate();
		
		
		
	}
}