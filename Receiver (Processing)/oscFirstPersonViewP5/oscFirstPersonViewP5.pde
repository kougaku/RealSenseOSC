import oscP5.*;

OscP5 oscP5;
float scale_factor = 1000; // [m] -> [mm]
PMatrix3D headMatrix = new PMatrix3D();
PMatrix3D offsetMatrix = new PMatrix3D();

void setup() {
  size(1400, 1000, P3D);
  oscP5 = new OscP5(this, 12000);
}

void draw() {
  background(50);

  // set camera state  
  perspective();
  setCamera();

  // wireframe box
  translate(0, 0, -600);
  noFill();
  stroke(255);
  strokeWeight(3);
  box(500);
  textSize(50);
  fill(255);
  textAlign(CENTER);
  text("First Person View", 0, 0);

  // text on 2D plane
  pushMatrix();
    ortho();
    camera();
    textSize(20);
    textAlign(LEFT);
    text("Hit space-key to reset position.", 20, 40);
  popMatrix();
}

void keyPressed() {
  if ( key == ' ' ) {
    resetCamera();
  }
}

void oscEvent(OscMessage msg) {
  // position
  float tx = scale_factor * msg.get(0).floatValue();
  float ty = scale_factor * msg.get(1).floatValue();
  float tz = scale_factor * msg.get(2).floatValue();

  // quaternion
  float qw = msg.get(3).floatValue();
  float qx = msg.get(4).floatValue();
  float qy = msg.get(5).floatValue();
  float qz = msg.get(6).floatValue();

  headMatrix = calcHeadMatrix(tx, ty, tz, qw, qx, qy, qz);
}

void setCamera() {
  PMatrix3D camera = ((PGraphicsOpenGL)g).camera;
  camera.reset();
  camera.apply(headMatrix);
  camera.apply(offsetMatrix);
}

void resetCamera() {
  // calculate the offset to the origin
  offsetMatrix = headMatrix.get();
  offsetMatrix.invert();
}

private PMatrix3D calcHeadMatrix(float tx, float ty, float tz, float qw, float qx, float qy, float qz) {
  PMatrix3D mat = new PMatrix3D();

  // calculate matrix terms
  float two_xSquared = 2 * qx * qx;
  float two_ySquared = 2 * qy * qy;
  float two_zSquared = 2 * qz * qz;
  float two_xy = 2 * qx * qy;
  float two_wz = 2 * qw * qz;
  float two_xz = 2 * qx * qz;
  float two_wy = 2 * qw * qy;
  float two_yz = 2 * qy * qz;
  float two_wx = 2 * qw * qx;

  // update view matrix orientation
  mat.m00 = 1 - two_ySquared - two_zSquared;
  mat.m01 = two_xy + two_wz;
  mat.m02 = two_xz - two_wy;
  mat.m10 = two_xy - two_wz;
  mat.m11 = 1 - two_xSquared - two_zSquared;
  mat.m12 = two_yz + two_wx;
  mat.m20 = two_xz + two_wy;
  mat.m21 = two_yz - two_wx;
  mat.m22 = 1 - two_xSquared - two_ySquared;
  mat.translate(-tx, -ty, -tz);

  // change right-hand to left-hand
  mat.preApply(
    1, 0, 0, 0, 
    0, -1, 0, 0, 
    0, 0, 1, 0, 
    0, 0, 0, 1
    );
  mat.scale(1, -1, 1);

  return mat;
}
