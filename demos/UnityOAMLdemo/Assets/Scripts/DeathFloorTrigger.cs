using UnityEngine;
using System.Collections;

public class DeathFloorTrigger : MonoBehaviour {
	private Player plr;

	// Use this for initialization
	void Start () {
		plr = GameObject.FindGameObjectWithTag ("Player").GetComponent<Player>();
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnTriggerEnter2D(Collider2D col) {
		plr.Damage (5);
	}
}
