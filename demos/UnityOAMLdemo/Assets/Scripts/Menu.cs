using UnityEngine;
using System.Collections;

public class Menu : MonoBehaviour {
	public GameObject PauseUI;

	// Use this for initialization
	void Start () {
		PauseUI.SetActive (false);
	}

	// Update is called once per frame
	void Update () {
		if (Input.GetButtonDown ("Cancel")) {
			if (PauseUI.activeSelf == true) {
				Resume ();
			} else {
				Pause ();
			}
		}
	}

	public void Resume() {
		PauseUI.SetActive (false);
		Time.timeScale = 1;
	}

	public void Pause() {
		PauseUI.SetActive (true);
		Time.timeScale = 0;
	}

	public void Restart() {
		Resume ();
		Application.LoadLevel (Application.loadedLevel);
	}

	public void Exit() {
		Application.Quit ();
	}
}
