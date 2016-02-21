using UnityEngine;
using System.Collections;

public class NextTrackTrigger : MonoBehaviour {
	private Music music;

	// Use this for initialization
	void Start () {
		music = GetComponentInParent<Music> ();
	}
	
	// Update is called once per frame
	void Update () {
	
	}

	void OnTriggerEnter2D(Collider2D col) {
		if (col.CompareTag ("Player")) {
			music.ToggleDay ();
		}
	}
}
