package four.test;

import javax.faces.application.FacesMessage;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.component.UIPanel;
import javax.faces.context.FacesContext;

@ManagedBean
@SessionScoped
public class CardController {
	private Card card;
	private UIPanel resultPanel;
	private int result;

	public CardController() {
		card = new Card();
	}

	public Card getCard() {
		return card;
	}

	public void setCard(Card card) {
		this.card = card;
	}
	
	public UIPanel getResultPanel() {
		return resultPanel;
	}

	public void setResultPanel(UIPanel resultPanel) {
		this.resultPanel = resultPanel;
	}
	
	public int getResult() {
		return result;
	}

	public void setResult(int result) {
		this.result = result;
	}
	
	public String checkResult() {
		FacesContext context = FacesContext.getCurrentInstance();
		resultPanel.setRendered(true);

		if (checkOperation()) {
			context.addMessage(null, new FacesMessage(FacesMessage.SEVERITY_INFO, "Correct", null));
		} else {
			context.addMessage(null, new FacesMessage(FacesMessage.SEVERITY_INFO, "Incorrect", null));
		}
		return null;
	}

	private boolean checkOperation() {
		return (card.getLeft() * card.getRight() == result);
	}

	public String next() {
		FacesContext context = FacesContext.getCurrentInstance();
		card = new Card();
		if (checkOperation()) {
			resultPanel.setRendered(false);
			return null;
		} else {
			context.addMessage(null, new FacesMessage(FacesMessage.SEVERITY_INFO, "Incorrect", null));
		}
		return null;

	}
	
}