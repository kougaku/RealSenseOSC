

class RealSenseT265 {
  PImage img_front;
  PImage img_back;

  RealSenseT265() {
    img_front = loadImage("T265_front.png");
    img_back = loadImage("T265_back.png");
  }

  void draw() {
    pushMatrix();
    rotateY(PI);
    
    noStroke();
    stroke(255);
    strokeWeight(1);
    fill(190);

    beginShape();
    texture(img_front);
    vertex( -54.0, -12.25, 6.25, 0, 0 );
    vertex(  54.0, -12.25, 6.25, img_front.width, 0 );
    vertex(  54.0, 12.25, 6.25, img_front.width, img_front.height );
    vertex( -54.0, 12.25, 6.25, 0, img_front.height );
    endShape();

    beginShape();
    texture(img_back);
    vertex(  54.0, -12.25, -6.25, 0, 0 );
    vertex( -54.0, -12.25, -6.25, img_back.width, 0 );
    vertex( -54.0, 12.25, -6.25, img_back.width, img_back.height );
    vertex(  54.0, 12.25, -6.25, 0, img_back.height );
    endShape();

    beginShape();
    vertex( -54.0, -12.25, -6.25);
    vertex(  54.0, -12.25, -6.25);
    vertex(  54.0, -12.25, 6.25);
    vertex( -54.0, -12.25, 6.25);
    endShape();

    beginShape();
    vertex( -54.0, 12.25, -6.25);
    vertex(  54.0, 12.25, -6.25);
    vertex(  54.0, 12.25, 6.25);
    vertex( -54.0, 12.25, 6.25);
    endShape();

    beginShape();
    vertex( -54.0, -12.25, -6.25);
    vertex( -54.0, -12.25, 6.25);
    vertex( -54.0, 12.25, 6.25);
    vertex( -54.0, 12.25, -6.25);
    endShape();

    beginShape();
    vertex( 54.0, -12.25, -6.25);
    vertex( 54.0, -12.25, 6.25);
    vertex( 54.0, 12.25, 6.25);
    vertex( 54.0, 12.25, -6.25);
    endShape();

    popMatrix();
  }
}
