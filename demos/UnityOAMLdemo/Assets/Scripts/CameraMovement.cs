using UnityEngine;
using System.Collections;

public class CameraMovement : MonoBehaviour {
	private Vector2 velocity;
	public GameObject plr;

	public float smoothTimeX;
	public float smoothTimeY;

	// Use this for initialization
	void Start () {
		plr = GameObject.FindGameObjectWithTag ("Player");
	}
	
	// Update is called once per frame
	void Update () {
		float posX = Mathf.SmoothDamp (transform.position.x, plr.transform.position.x, ref velocity.x, smoothTimeX);
	//	float posY = Mathf.SmoothDamp (transform.position.y, plr.transform.position.y, ref velocity.y, smoothTimeY);

		transform.position = new Vector3 (posX, transform.position.y, transform.position.z);
	}
}
