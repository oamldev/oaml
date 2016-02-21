using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class HUD : MonoBehaviour {
	public Sprite[] HealthSprites;
	public Image HealthUI;
	private Player plr;
	
	// Use this for initialization
	void Start () {
		plr = GameObject.FindGameObjectWithTag ("Player").GetComponent<Player>();
	}
	
	// Update is called once per frame
	void Update () {
		if (plr.curHP >= 0 && plr.curHP <= 5)
			HealthUI.sprite = HealthSprites [plr.curHP];
	}
}
