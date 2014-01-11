package model;

import java.io.Serializable;
import javax.persistence.*;


/**
 * The persistent class for the usersextra database table.
 * 
 */
@Entity
@NamedQuery(name="Usersextra.findAll", query="SELECT u FROM Usersextra u")
public class Usersextra implements Serializable {
	private static final long serialVersionUID = 1L;

	@Id
	@Column(name="user_id")
	private int userId;

	private String address;

	@Lob
	private String bio;

	private String city;

	@Column(name="first_name")
	private String firstName;

	@Lob
	private String interests;

	@Column(name="last_name")
	private String lastName;

	private String postcode;

	private String province;

	@Lob
	private String signature;

	@Lob
	private String skills;

	public Usersextra() {
	}

	public int getUserId() {
		return this.userId;
	}

	public void setUserId(int userId) {
		this.userId = userId;
	}

	public String getAddress() {
		return this.address;
	}

	public void setAddress(String address) {
		this.address = address;
	}

	public String getBio() {
		return this.bio;
	}

	public void setBio(String bio) {
		this.bio = bio;
	}

	public String getCity() {
		return this.city;
	}

	public void setCity(String city) {
		this.city = city;
	}

	public String getFirstName() {
		return this.firstName;
	}

	public void setFirstName(String firstName) {
		this.firstName = firstName;
	}

	public String getInterests() {
		return this.interests;
	}

	public void setInterests(String interests) {
		this.interests = interests;
	}

	public String getLastName() {
		return this.lastName;
	}

	public void setLastName(String lastName) {
		this.lastName = lastName;
	}

	public String getPostcode() {
		return this.postcode;
	}

	public void setPostcode(String postcode) {
		this.postcode = postcode;
	}

	public String getProvince() {
		return this.province;
	}

	public void setProvince(String province) {
		this.province = province;
	}

	public String getSignature() {
		return this.signature;
	}

	public void setSignature(String signature) {
		this.signature = signature;
	}

	public String getSkills() {
		return this.skills;
	}

	public void setSkills(String skills) {
		this.skills = skills;
	}

}