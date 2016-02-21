using UnityEngine;
using System.Collections;

public class GroundCheck : MonoBehaviour {
	private Player plr;

	// Use this for initialization
	void Start () {
		plr = gameObject.GetComponentInParent<Player> ();
	
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnTriggerEnter2D(Collider2D col) {
		plr.grounded = true;
	}
	
	void OnTriggerStay2D(Collider2D col) {
		plr.grounded = true;
	}
	
	void OnTriggerExit2D(Collider2D col) {
		plr.grounded = false;
	}
}
