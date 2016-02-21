using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public class Spikes : MonoBehaviour {
	[DllImport ("AudioPluginOAML")]
	private static extern void oamlAddTension (int value);

	private Player plr;
	private Music music;
	
	// Use this for initialization
	void Start () {
		plr = GameObject.FindGameObjectWithTag ("Player").GetComponent<Player>();
	}
	
	// Update is called once per frame
	void Update () {
		
	}
	
	void OnTriggerEnter2D(Collider2D col) {
		if (col.CompareTag ("Player")) {
			plr.Damage(1);
			plr.KnockBack(350f);
			oamlAddTension (10);
		}
	}
}
