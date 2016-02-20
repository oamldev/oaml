using UnityEngine;
using System.Collections;

public class Parallax : MonoBehaviour {
	public Transform[] dynamicBgs;
	public Transform staticBg;

	public float bgStartX;
	public float smoothing;

	private Transform cam;
	private float[] scales;
	private Vector3 prevCamPos;

	// Use this for initialization
	void Start () {
		cam = Camera.main.transform;

		prevCamPos = cam.position;
		scales = new float[dynamicBgs.Length];
		for (int i = 0; i < dynamicBgs.Length; i++) {
			scales [i] = 50f - (dynamicBgs [i].position.z * -1);
		}
	}
	
	// Update is called once per frame
	void LateUpdate () {
		float parallax;
		float posX;
		Vector3 pos;

		for (int i = 0; i < dynamicBgs.Length; i++) {
			parallax = (prevCamPos.x - cam.position.x) * scales[i];
			posX = dynamicBgs [i].position.x - parallax;
			pos = new Vector3 (posX, dynamicBgs [i].position.y, dynamicBgs [i].position.z);
			dynamicBgs [i].position = Vector3.Lerp(dynamicBgs [i].position, pos, smoothing * Time.deltaTime);
		}

		posX = cam.position.x - (cam.position.x - bgStartX) * 0.04f;
		staticBg.position = new Vector3(posX, staticBg.position.y, staticBg.position.z);

		prevCamPos = cam.position;
	}
}
