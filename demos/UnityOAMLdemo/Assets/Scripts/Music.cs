using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections;
using System.Runtime.InteropServices;

public class Music : MonoBehaviour {
	[DllImport ("AudioPluginOAML")]
	private static extern int oamlInit (string defsFilename);

	[DllImport ("AudioPluginOAML")]
	private static extern int oamlPlayTrack (string name);

	[DllImport ("AudioPluginOAML")]
	private static extern void oamlSetMainLoopCondition (int value);

	[DllImport ("AudioPluginOAML")]
	private static extern void oamlPause ();

	[DllImport ("AudioPluginOAML")]
	private static extern void oamlResume ();

	[DllImport ("AudioPluginOAML")]
	private static extern IntPtr oamlGetPlayingInfo ();

	private Text textLog;
	private SpriteRenderer background;
	private bool day = true;

	// Use this for initialization
	void Start () {
		textLog = GameObject.FindGameObjectWithTag ("LogText").GetComponent<Text>();
		background = GameObject.FindGameObjectWithTag ("FarAway").GetComponent<SpriteRenderer>();

		oamlInit ("oaml.defs");
		oamlPlayTrack ("8bit");
		oamlResume ();
		oamlSetMainLoopCondition (1);
	}

	void OnDisable () {
		oamlPause ();
	}

	void OnEnable () {
		oamlResume ();
	}

	// Update is called once per frame
	void Update () {
		textLog.text = Marshal.PtrToStringAnsi(oamlGetPlayingInfo());
		if (day) {
			textLog.text += "\nGame time is day";
		} else {
			textLog.text += "\nGame time is night";
		}
	}

	public void Play8bitTrack() {
		oamlPlayTrack ("8bit");
	}

	public void PlayOrchestralTrack() {
		oamlPlayTrack ("Orchestral");
	}

	public void ToggleDay()  {
		if (day) {
			day = false;
			background.color = new Color (0.1f, 0.1f, 0.1f, 1f);
			oamlSetMainLoopCondition (20);
		} else {
			day = true;
			background.color = new Color (1f, 1f, 1f, 1f);
			oamlSetMainLoopCondition (1);
		}
	}
}
