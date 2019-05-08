import oscP5.*;
OscP5 oscP5;

RealSenseT265 model;

float tx, ty, tz, rx, ry, rz;
float offset_tx = 0;
float offset_ty = 0;
float offset_tz = 0;

float eyeX = 100;
float eyeY = 0;
float eyeZ = 500;
float baseRot = 0;

ArrayList<PVector> position = new ArrayList<PVector>();

void setup() {
  size(1400, 1000, P3D);
  oscP5 = new OscP5(this, 12000);
  model = new RealSenseT265();
}

void draw() {
  background(50);

  perspective();
  pushMatrix(); 
  {
    camera( eyeX, eyeY, eyeZ, 0, 0, 0, 0, 1, 0 );
    rotateY(radians(baseRot));

    // wire frame
    noFill();
    stroke(255);
    strokeWeight(3);
    box(500);

    // offset
    translate( -offset_tx, -offset_ty, -offset_tz );

    // draw trajectory
    for (int i=0; i<position.size()-1; i++) {
      PVector p1 = position.get(i);
      PVector p2 = position.get(i+1);
      stroke(0, 255, 0);
      strokeWeight(5);
      line( p1.x, p1.y, p1.z, p2.x, p2.y, p2.z );
    }

    // draw device model
    translate( tx, ty, tz );
    rotateX(rx);
    rotateY(ry);
    rotateZ(rz);
    model.draw();
  }
  popMatrix();

  // text
  ortho();
  fill(0, 255, 255);
  textSize(20);
  text("count:" + position.size() + "  x=" + round(tx) + ", y=" + round(ty) + ", z=" + round(tz) + " [mm]", 10, 30);
  text("[SPACE] reset position", 10, 60);
  text("[UP][DOWN] move vertically", 10, 90 );
  text("[LEFT][RIGHT] rotate", 10, 120 );

  // Viewpoint control
  if ( keyPressed ) {
    if ( keyCode == UP ) {
      eyeY -= 10;
    }
    if ( keyCode == DOWN ) {
      eyeY +=10;
    }
    if ( keyCode == LEFT ) {
      baseRot += 5;
    }
    if ( keyCode == RIGHT ) {
      baseRot -= 5;
    }
  }
}

void keyPressed() {
  if (key == ' ') {
    position.clear();
    offset_tx = tx;
    offset_ty = ty;
    offset_tz = tz;
  }
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

  // position log
  if ( position.size() < 30000 ) {
    position.add( new PVector( tx, ty, tz ) );
  }
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
