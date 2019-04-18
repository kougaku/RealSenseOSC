import oscP5.*;
OscP5 oscP5;

float tx, ty, tz, rx, ry, rz;

void setup() {
  size(1400, 1000, P3D);
  oscP5 = new OscP5(this, 7000);
}

void draw() {
  background(50);

  camera( 0, 0, 400, 0, 0, 0, 0, 1, 0 );

  // wire frame
  noFill();
  stroke(255);
  strokeWeight(3);
  box(500);

  // draw device
  translate( tx, ty, tz );
  rotateX(rx);
  rotateY(ry);
  rotateZ(rz);
  fill(200);
  box(108, 24.5, 12.5);
}

void oscEvent(OscMessage msg) {
  // position
  float scale_f = 1000; // [m]->[mm]
  tx =  scale_f * msg.get(0).floatValue();
  ty = -scale_f * msg.get(1).floatValue();
  tz =  scale_f * msg.get(2).floatValue();

  // quaternion
  float qw = msg.get(3).floatValue();
  float qx = msg.get(4).floatValue();
  float qy = msg.get(5).floatValue();
  float qz = msg.get(6).floatValue();

  // quaternion -> euler angle
  float [] euler = convQuaternionToEuler(qx, -qy, qz, qw);  
  rx = -euler[0];
  ry = -euler[1];
  rz = -euler[2];
}

float[] convQuaternionToEuler(float x, float y, float z, float w) {
  float ysqr = y * y;
  float t0 = 2.0 * (w * x + y * z);
  float t1 = 1.0 - 2.0 * (x * x + ysqr);
  float euler_x = atan2(t0, t1);

  float t2 = 2.0 * (w * y - z * x);
  t2 = (t2 > 1.0) ? 1.0 : t2;
  t2 = (t2 < -1.0) ? -1.0 : t2;
  float euler_y = asin(t2);

  float t3 = 2.0 * (w * z + x * y);
  float t4 = 1.0 - 2.0 * (ysqr + z * z);
  float euler_z = atan2(t3, t4);

  float[] out = new float [3];
  out[0] = euler_x;
  out[1] = euler_y;
  out[2] = euler_z;

  return out;
}
