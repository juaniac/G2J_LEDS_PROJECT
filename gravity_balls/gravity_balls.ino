#include <FastLED.h>

#define LED_PIN 2 

#define BRIGHTNESS  100  //64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

#define nbLeds 202
CRGB leds[nbLeds];

#define nbLeds 202

struct Ball{
  int velocity;
  uint16_t pos;
  struct EdgeInfo* edgeInfo;
};

struct EAD{
  uint16_t start;
  uint16_t end;
  int dir;
};

struct AdjacentEdges{
  uint16_t nbAdEdges;
  uint16_t* conPoints;
  struct EdgeInfo** adEdgesInfo;
};

struct EdgeInfo{
  EAD* ptrEdge;
  AdjacentEdges adjacentEdgesLeft;
  AdjacentEdges adjacentEdgesRight;
};

#define nbStartingPoints 2
uint16_t startingPoints[] = {101,102};
EdgeInfo* startingPointsInfo[nbStartingPoints];

#define nbMaxConnections 2
#define notInUse nbLeds
#define nbConnectedPoints 4
uint16_t connectedPoints[nbConnectedPoints][nbMaxConnections] ={{0,201}, {44,45}, {101, 102}, {157, 158}};

#define nbEdges 6
EAD edges[] = {{0, 44, 1},{45, 66, -1},{66, 101, 1},{102, 136, -1},{136, 157, 1},{158, 201, -1}};
EdgeInfo edgesInfo[nbEdges];

#define nbBalls 1
Ball* ballList[nbBalls];
#define spawnBallEvery 1
uint16_t spawnTimer = 0;
#define blockSize 10

#define Gravity -10 //in mm/s^2

#define UPS 240

void setup() {
    delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, nbLeds).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
    Serial.begin(9600);

    startUpGravity();
}

void loop()
{
    updateBalls();
    FastLED.show();
    FastLED.delay(1000 /UPS);
}

  void startUpGravity(){
    for(uint16_t i = 0; i < nbEdges; i++){
      edgesInfo[i].ptrEdge = &edges[i];
    }
    for(uint16_t i = 0; i < nbEdges; i++){
      uint16_t curIndexStart = edges[i].start;
      uint16_t nbAdEdgesStart = 0;
      uint16_t* conPointsStart = NULL;
      EdgeInfo** adEdgesInfoStart = NULL;

      uint16_t curIndexEnd = edges[i].end;
      uint16_t nbAdEdgesEnd = 0;
      uint16_t* conPointsEnd = NULL;
      EdgeInfo** adEdgesInfoEnd = NULL;
      
      if(curIndexStart == edges[constrain(i-1, 0, nbEdges-1)].end){
        nbAdEdgesStart += 1;
        conPointsStart = realloc(conPointsStart, nbAdEdgesStart * sizeof(uint16_t));
        adEdgesInfoStart = realloc(adEdgesInfoStart, nbAdEdgesStart * sizeof(EdgeInfo*));
        conPointsStart[nbAdEdgesStart-1] = curIndexStart;
        adEdgesInfoStart[nbAdEdgesStart-1] = &edgesInfo[i-1];
      }
      if(curIndexEnd == edges[constrain(i+1, 0, nbEdges-1)].start){
        nbAdEdgesEnd += 1;
        conPointsEnd = realloc(conPointsEnd, nbAdEdgesEnd * sizeof(uint16_t));
        adEdgesInfoEnd = realloc(adEdgesInfoEnd, nbAdEdgesEnd * sizeof(EdgeInfo*));
        conPointsEnd[nbAdEdgesEnd-1] = curIndexEnd;
        adEdgesInfoEnd[nbAdEdgesEnd-1] = &edgesInfo[i+1];
      }
      connectedPointsSearch(curIndexStart, &nbAdEdgesStart, &adEdgesInfoStart, &conPointsStart);
      connectedPointsSearch(curIndexEnd, &nbAdEdgesEnd, &adEdgesInfoEnd, &conPointsEnd);

      edgesInfo[i].adjacentEdgesLeft = {nbAdEdgesStart, conPointsStart, adEdgesInfoStart};
      edgesInfo[i].adjacentEdgesRight = {nbAdEdgesEnd, conPointsEnd, adEdgesInfoEnd};    
    }
    
    for(uint16_t i = 0; i < nbStartingPoints; i++){
      uint16_t startPoint = startingPoints[i];
      EdgeInfo* ptrEdgeInfo = NULL;
      for(uint16_t j = 0; j < nbEdges && ptrEdgeInfo == NULL; j++){
        if(edges[j].start == startPoint || edges[j].end == startPoint) ptrEdgeInfo = &edgesInfo[j];
      }
      startingPointsInfo[i] = ptrEdgeInfo;
    }
  }

  void connectedPointsSearch(uint16_t curIndex, uint16_t* nbAdEdges, EdgeInfo*** adEdgesInfo, uint16_t** conPoints){
     uint16_t curIndexInConPoints = notInUse;
      for(uint16_t j = 0; j < nbConnectedPoints && curIndexInConPoints == notInUse; j++){
        uint16_t* curConPoints = connectedPoints[j];

        for(uint16_t k = 0; k < nbMaxConnections && curIndexInConPoints == notInUse && curConPoints[k] != notInUse; k++){
          if(curConPoints[k] == curIndex){
            curIndexInConPoints = k;
          }
        }

        for(uint16_t k = 0; k < nbMaxConnections && curIndexInConPoints != notInUse && curConPoints[k] != notInUse; k++){
          if(k != curIndexInConPoints){
            uint16_t conPoint = curConPoints[k];
            *nbAdEdges += 1;
            *adEdgesInfo = realloc(*adEdgesInfo, (*nbAdEdges) * sizeof(EdgeInfo*));
            *conPoints = realloc(*conPoints, (*nbAdEdges) * sizeof(uint16_t));
            uint16_t conPointInEdges = notInUse;
            for(uint16_t l = 0; l < nbEdges && conPointInEdges == notInUse; l++){
              if(conPoint == edges[l].start || conPoint == edges[l].end) conPointInEdges = l;
            }
            (*conPoints)[*nbAdEdges-1] = conPoint;
            (*adEdgesInfo)[*nbAdEdges-1] = &edgesInfo[conPointInEdges];
          }
        }
      } 
  }

  void updateBalls(){
    bool updatedTimer = false;
    for(uint16_t i = 0 ; i < nbBalls; i++){
      Ball* curBall = ballList[i]; 
      if(curBall == NULL && !updatedTimer){
        spawnTimer += 1;
        updatedTimer = true;
        spawnBall(&curBall);   
        ballList[i] = curBall;
      }else if(curBall != NULL){  
        leds[curBall->pos] = CRGB(0,0,0);   
        //if(curBall->velocity == 0){
        //  free(curBall);
        //  ballList[i] = NULL;
        //}else{
          updatePosition(&curBall);
          curBall->velocity = constrain(curBall->velocity - 10, -10*UPS, 10*UPS) -constrain(curBall->velocity - 10, -1, 1); //(Gravity*10)/UPS, -10, 10); // in mm/s
          leds[curBall->pos] = CRGB(255,255,255);
        //}
      }
    }
    debugBallInfo();
  }

  void spawnBall(Ball** ptrBall){
    if(spawnTimer > (UPS) * (spawnBallEvery) ){
      spawnTimer = 0;
      while((*ptrBall) == NULL){(*ptrBall) = calloc(1, sizeof(Ball));}
        uint8_t randomIndex = random8(0,nbStartingPoints);
        (*ptrBall)->velocity = 30;
        (*ptrBall)->pos = startingPoints[randomIndex];
        (*ptrBall)->edgeInfo = startingPointsInfo[randomIndex];
        leds[(*ptrBall)->pos] = CRGB(255,255,255);
    }
  }

  void updatePosition(Ball** ptrBall){
    int dir = (*ptrBall)->edgeInfo->ptrEdge->dir;
    uint16_t start = (*ptrBall)->edgeInfo->ptrEdge->start;
    uint16_t end = (*ptrBall)->edgeInfo->ptrEdge->end;
    int vel = (*ptrBall)->velocity;
    uint16_t pos = (*ptrBall)->pos;

    //change here to make the ball slower;
    //int newPosition = (dir*Gravity + 2*dir*vel*UPS + 2*pos*UPS*UPS)/(2*UPS*UPS);
    int newPosition = (dir*vel)/(UPS) + pos;

    if(start <= newPosition && newPosition <= end){
      (*ptrBall)->pos = (uint16_t)(newPosition);
    }else if((int)(start) > newPosition){
      if((*ptrBall)->edgeInfo->adjacentEdgesLeft.nbAdEdges != 0){
        uint16_t randomIndex = random16(0, (*ptrBall)->edgeInfo->adjacentEdgesLeft.nbAdEdges);
        uint16_t concPoint = (*ptrBall)->edgeInfo->adjacentEdgesLeft.conPoints[randomIndex];
        (*ptrBall)->edgeInfo = (*ptrBall)->edgeInfo->adjacentEdgesLeft.adEdgesInfo[randomIndex];
        int newDir = (*ptrBall)->edgeInfo->ptrEdge->dir;
        if(concPoint != start){
          newPosition = (dir == newDir ? -(newPosition - start) : (newPosition - start)) + concPoint;
        }
        (*ptrBall)->pos = newPosition;
        (*ptrBall)->velocity = dir == newDir ? vel : -1*vel;
      }else{
        (*ptrBall)->velocity = -1*vel;
        (*ptrBall)->pos = -(newPosition - start) + start;
      }
    }else{
      if((*ptrBall)->edgeInfo->adjacentEdgesRight.nbAdEdges != 0){
        uint16_t randomIndex = random16(0, (*ptrBall)->edgeInfo->adjacentEdgesRight.nbAdEdges);
        uint16_t concPoint = (*ptrBall)->edgeInfo->adjacentEdgesRight.conPoints[randomIndex];
        (*ptrBall)->edgeInfo = (*ptrBall)->edgeInfo->adjacentEdgesRight.adEdgesInfo[randomIndex];
        int newDir = (*ptrBall)->edgeInfo->ptrEdge->dir;

        if(concPoint != end){
          newPosition = (dir == newDir ? (end - newPosition) : -(end - newPosition)) + concPoint;
        }
        (*ptrBall)->velocity = dir == newDir ? vel : -1*vel;
        (*ptrBall)->pos = newPosition;
      }else{
        (*ptrBall)->velocity = -1*vel;
        (*ptrBall)->pos = (end - newPosition) + end;
      }
    }
  }

  void debugEdgeInfoPrint(){
    Serial.println("");
    Serial.println("START");
    for(int i = 0; i < nbEdges; i++){
      EAD* curEdge = edgesInfo[i].ptrEdge;
      printEAD(curEdge->start, curEdge->end);
      Serial.println("");
      Serial.print("LEFT: ");
      for(int j = 0; j < edgesInfo[i].adjacentEdgesLeft.nbAdEdges; j++){
        curEdge = edgesInfo[i].adjacentEdgesLeft.adEdgesInfo[j]->ptrEdge;
        printEAD(curEdge->start, curEdge->end);
        Serial.print(", ");
      }
      Serial.println("");
      Serial.print("RIGHT: ");
      for(int j = 0; j < edgesInfo[i].adjacentEdgesRight.nbAdEdges; j++){
        curEdge = edgesInfo[i].adjacentEdgesRight.adEdgesInfo[j]->ptrEdge;
        printEAD(curEdge->start, curEdge->end);
        Serial.print(", ");
      }
      Serial.println("");
    }
  }
  void debugBallInfo(){
    Serial.println("");
    Serial.println("START");
    for(int i = 0; i < nbBalls && ballList[i] != NULL; i++){
      Serial.print("ball NB:");Serial.println(i);
      Serial.print("VELOCITY:");Serial.println(ballList[i]->velocity);
      Serial.print("POSITION:");Serial.println(ballList[i]->pos);
      Serial.print("CURENT EDGE:");  printEAD(ballList[i]->edgeInfo->ptrEdge->start, ballList[i]->edgeInfo->ptrEdge->end);
      Serial.println("");
    }
  }

  void printEAD(uint16_t start, uint16_t end){
    Serial.print("{"); Serial.print(start); Serial.print(", "); Serial.print(end); Serial.print("}");
  }