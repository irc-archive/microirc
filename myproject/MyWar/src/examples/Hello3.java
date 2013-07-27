package examples;

import java.io.IOException;

import javax.servlet.ServletException;
import javax.servlet.annotation.WebServlet;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

@WebServlet("/hello3")
public class Hello3 extends HttpServlet {
	
	private static final long serialVersionUID = 4997429776740173060L;

	public Hello3() {
		// TODO Auto-generated constructor stub
	}
	
	@Override
	protected void doGet(HttpServletRequest request, HttpServletResponse reply)
			throws ServletException, IOException {
		// TODO Auto-generated method stub
		reply.getWriter().append("hello3");
		reply.getWriter().flush();
	}
	
}
