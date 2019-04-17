
import oscP5.*;
import netP5.*;

OscP5 oscP5;

float tx, ty, tz, qw, qx, qy, qz;

void setup() {
  size(1500, 1000, P3D);
  oscP5 = new OscP5(this, 7000);
}

void draw() {
  background(50);

  noFill();
  stroke(255);
  strokeWeight(3);

  camera( 0, 0, 500, 0, 0, 0, 0, 1, 0 );
  rotateY(radians(45));

  pushMatrix(); 
  {
    box(1000);

    fill(255, 0, 0);
    float s = 300;
    translate( s*tx, -s*ty, s*tz );
    //box(50);
    
    float [] euler = quaternion_to_euler_angle(qx, -qy, qz, qw);
    rotateX(-euler[0]);
    rotateY(-euler[1]);
    rotateZ(-euler[2]);
    
    fill(200);
    box(250, 50, 20);
  } 
  popMatrix();
}

void oscEvent(OscMessage msg) {
  // print("### received an osc message.");
  // print(" addrpattern: " + msg.addrPattern());
  // println(" typetag: " + msg.typetag());

  tx = msg.get(0).floatValue();
  ty = msg.get(1).floatValue();
  tz = msg.get(2).floatValue();
  qw = msg.get(3).floatValue();
  qx = msg.get(4).floatValue();
  qy = msg.get(5).floatValue();
  qz = msg.get(6).floatValue();

  println( tx, ty, tz, qw, qx, qy, qz );
}

// code from:
// https://toxweblog.toxbe.com/2018/07/22/processing-3d-rotate-with-quaternion/

float[] quaternion_to_euler_angle(float x, float y, float z, float w) {
  float ysqr = y * y;

  float t0 = 2.0 * (w * x + y * z);
  float t1 = 1.0 - 2.0 * (x * x + ysqr);
  float X = atan2(t0, t1);

  float t2 = 2.0 * (w * y - z * x);
  t2 = (t2 > 1.0)? 1.0:t2;
  t2 = (t2 < -1.0)? -1.0:t2;
  float Y = asin(t2);

  float t3 = 2.0 * (w * z + x * y);
  float t4 = 1.0 - 2.0 * (ysqr + z * z);

  float Z = atan2(t3, t4);

  float[] forReturn = new float[3];
  forReturn[0] = X;
  forReturn[1] = Y;
  forReturn[2] = Z;
  return forReturn;
}
