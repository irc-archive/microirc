package examples;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class Hello extends HttpServlet {

	private static final long serialVersionUID = -7012635316331048093L;

	@Override
	protected void doGet(HttpServletRequest request, HttpServletResponse reply)
			throws ServletException, IOException {
		// TODO Auto-generated method stub
		reply.getWriter().append("aaaa");
		reply.getWriter().flush();
	}
	
}
