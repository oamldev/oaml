using UnityEngine;
using System.Collections;

public class Player : MonoBehaviour {
	public float maxSpeed;
	public float speed;
	public float jumpPower;

	public bool grounded;

	public int curHP;
	public int maxHP;
	public bool alive;

	private Rigidbody2D body;
	private Animator anim;

	private AudioSource jumpSFX;
	private AudioSource loseSFX;
	private AudioSource hitSFX;

	// Use this for initialization
	void Start () {
		AudioSource[] audios = GetComponents<AudioSource>();

		body = gameObject.GetComponent<Rigidbody2D>();
		anim = gameObject.GetComponent<Animator>();

		jumpSFX = audios [0];
		loseSFX = audios [1];
		hitSFX = audios [2];

		maxSpeed = 3f;
		speed = 50f;
		jumpPower = 350f;
		maxHP = 5;
		curHP = maxHP;
		alive = true;
	}
	
	// Update is called once per frame
	void Update () {
		anim.SetBool ("Grounded", grounded);
		anim.SetFloat ("Speed", Mathf.Abs (Input.GetAxis ("Horizontal")));

		if (grounded == true && Input.GetButtonDown ("Jump")) {
			Jump();
		}

		if (Input.GetAxis ("Horizontal") < 0)  {
			transform.localScale = new Vector3(-1, 1, 1);
		} else if (Input.GetAxis ("Horizontal") > 0)  {
			transform.localScale = new Vector3(1, 1, 1);
		}
	}

	void FixedUpdate() {
		if (grounded) {
			Vector3 vel = body.velocity;
			vel.x*= 0.75f;

			body.velocity = vel;
		}

		float h = Input.GetAxis("Horizontal");

		body.AddForce((Vector2.right * speed) * h);
		if (body.velocity.x > maxSpeed)
			body.velocity = new Vector2(maxSpeed, body.velocity.y);
		if (body.velocity.x < -maxSpeed)
			body.velocity = new Vector2(-maxSpeed, body.velocity.y);
	}

	void Jump() {
		body.AddForce (Vector2.up * jumpPower);
		jumpSFX.Play ();
	}

	public void Damage(int amt) {
		SpriteRenderer sprite = GetComponent<SpriteRenderer>();
		sprite.color = new Color (1f, 1f, 1f, 1f);
		StartCoroutine(FlashSprite(sprite, 10, 0.1f, new Color(1f, 0f, 0f, 0.5f)));
//		curHP -= amt;
		
		if (curHP <= 0 && alive) {
			alive = false;
			loseSFX.Play ();
		} else {
			hitSFX.Play ();
		}
	}

	public void KnockBack (float knockBackPwr) {
		body.velocity = new Vector2(0, 0);
		body.AddForce(new Vector2(0, knockBackPwr));
	}﻿

	IEnumerator FlashSprite(SpriteRenderer sprite, int numTimes, float delay, Color c) {
		Color co = sprite.color;

		for (int loop = 0; loop < numTimes; loop++) {            
			sprite.color = c;
			yield return new WaitForSeconds(delay);
			
			sprite.color = co;
			yield return new WaitForSeconds(delay);
		}
	}
}
