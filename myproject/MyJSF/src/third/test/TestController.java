package third.test;

import java.io.Serializable;

import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;

@ManagedBean
@SessionScoped
public class TestController implements Serializable {

	private static final long serialVersionUID = 1L;

	public String getPage1() {
		return "success";
	}

	public String getPage2() {
		return "failed";
	}

}