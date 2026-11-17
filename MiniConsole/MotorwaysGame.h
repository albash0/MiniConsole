#pragma once
#include <stdint.h>
#include <string.h>
#include <initializer_list>

// Fixed-size simulation shared by the console and host tests. One tick = 50 ms.
struct MotorwaysGame {
  static constexpr int W=20,H=10,N=W*H,HOMES=12,DESTS=6,CARS=24,LINKS=3;
  static constexpr uint8_t NONE=255;
  enum Tool:uint8_t { ROAD,ERASE,EXPRESS };
  enum Result:uint8_t { OK,BLOCKED,NO_ROADS,NO_BRIDGE,NO_EXPRESS,BUSY,TOO_LONG };
  enum Event:uint8_t { DELIVERY=1,GROWTH=2,WEEK=4,FAILURE=8,WARNING=16,REDEVELOP=32,OPENED=64 };
  static constexpr uint16_t OPEN_GRACE=400,CLOSE_NOTICE=200,CLOSE_LIMIT=600;
  struct Home { uint8_t cell=0,color=0,cooldown=0; };
  struct Destination { uint8_t anchor=0,color=0,demand=1,flash=0; uint16_t clock=0,overload=0; };
  struct Car { uint8_t from=0,to=0,home=0,dest=0,progress=0,hold=0; bool active=false,returning=false; };
  struct Link { uint8_t a=0,b=0; };
  uint8_t roads[N]{},junction[N]{};
  Home homes[HOMES]; Destination destinations[DESTS]; Car cars[CARS]; Link links[LINKS];
  uint8_t homeCount=0,destCount=0,linkCount=0,bridges=1,express=0,difficulty=1,cursor=42;
  uint16_t stock=32; uint32_t rng=1,score=0;
  uint64_t ticks=0;
  uint16_t grace[DESTS]{},closeClock=0;
  uint8_t closing=NONE,replacement=NONE,replacementColor=0,nextRenew=0;
  uint32_t renewals=0;
  bool systemTheme=false;
  bool alive=true,rewardPending=false;
  Tool tool=ROAD;
  // Preview is transient: only confirmed roads belong in a checkpoint.
  bool editing=false; uint8_t plan[N]{}; uint16_t planLength=0;
  static constexpr uint8_t cell(int x,int y) { return uint8_t(y*W+x); }
  static int x(uint8_t p) { return p%W; }
  static int y(uint8_t p) { return p/W; }
  static bool water(uint8_t p) { return x(p)==10; }
  static uint8_t neighbor(uint8_t p,int d) {
    if(d<0||d>=8)return NONE;
    static constexpr int8_t dx[]={0,1,0,-1,1,1,-1,-1},dy[]={-1,0,1,0,-1,1,1,-1};
    int nx=x(p)+dx[d],ny=y(p)+dy[d];
    return nx<0||nx>=W||ny<0||ny>=H?NONE:cell(nx,ny);
  }
  static int direction(uint8_t a,uint8_t b) { for(int d=0;d<8;++d)if(neighbor(a,d)==b)return d;return -1; }
  static int opposite(int d){return d<4?(d+2)%4:4+(d-4+2)%4;}
  bool diagonalClear(uint8_t a,uint8_t b,const uint8_t *mask) const {
    if(x(a)==x(b)||y(a)==y(b))return true;
    uint8_t c=cell(x(a),y(b)),e=cell(x(b),y(a));
    if(building(c)||building(e))return false;
    return true;
  }
  static bool bridgeAxis(uint8_t mask){return !(mask&~10)||!(mask&~80)||!(mask&~160);}
  uint8_t port(int i) const { return uint8_t(destinations[i].anchor+W); }
  int homeAt(uint8_t p) const { for(int i=0;i<homeCount;++i)if(homes[i].cell==p)return i;return -1; }
  int destAt(uint8_t p) const {
    for(int i=0;i<destCount;++i){int dx=x(p)-x(destinations[i].anchor),dy=y(p)-y(destinations[i].anchor);if(dx>=0&&dx<2&&dy>=0&&dy<2)return i;}return -1;
  }
  static bool footprint(uint8_t anchor,uint8_t p) {
    return x(p)>=x(anchor)&&x(p)<x(anchor)+2&&y(p)>=y(anchor)&&y(p)<y(anchor)+2;
  }
  bool reserved(uint8_t p) const { return closing!=NONE&&replacement!=destinations[closing].anchor&&footprint(replacement,p); }
  bool building(uint8_t p) const { return homeAt(p)>=0||destAt(p)>=0||reserved(p); }
  bool blocked(uint8_t p) const { int i=destAt(p);return (i>=0&&p!=port(i))||(reserved(p)&&p!=replacement+W); }
  uint8_t snap(uint8_t p) const { int i=destAt(p);return i>=0?port(i):reserved(p)?uint8_t(replacement+W):p; }
  bool linked(uint8_t a,uint8_t b) const { for(int i=0;i<linkCount;++i)if((links[i].a==a&&links[i].b==b)||(links[i].a==b&&links[i].b==a))return true;return false; }
  uint32_t random() { rng^=rng<<13;rng^=rng>>17;rng^=rng<<5;return rng; }
  uint32_t week() const { uint64_t n=ticks/1200+1;return n>UINT32_MAX?UINT32_MAX:uint32_t(n); }
  uint16_t overloadLimit() const { return difficulty==0?180:difficulty==2?90:120; }
  uint16_t openingGrace() const { return difficulty==0?300:difficulty==2?160:200; }
  uint16_t initialStock() const { return difficulty==0?36:difficulty==2?24:28; }
  uint8_t roadReward() const { return 16; }
  uint8_t kitRoadReward(bool bridge) const { return difficulty==0?(bridge?8:6):difficulty==2?(bridge?4:3):(bridge?6:4); }
  uint8_t junctionDelay() const { return difficulty==0?8:difficulty==2?14:12; }
  static int manhattan(uint8_t a,uint8_t b) { int dx=x(a)-x(b),dy=y(a)-y(b);return (dx<0?-dx:dx)+(dy<0?-dy:dy); }
  void connect(uint8_t a,uint8_t b) { int d=direction(a,b);if(d>=0){roads[a]|=1<<d;roads[b]|=1<<opposite(d);} }
  void reset(uint32_t seed,uint8_t level=1) {
    *this=MotorwaysGame{}; rng=seed?seed:1;difficulty=level>2?1:level;
    stock=initialStock();
    // Randomize the entire opening, not just later growth. Rejection sampling
    // checks two complete routes sharing at most one bridge against the budget.
    bool placed=false;
    for(int attempt=0;attempt<96&&!placed;++attempt){
      homeCount=destCount=0;
      for(int i=0;i<2;++i){
        bool found=false;
        for(int n=0;n<N;++n){uint8_t p=uint8_t(random()%N);
          if(!siteAvailable(p)||(destCount&&manhattan(p,destinations[0].anchor)<5))continue;
          destinations[destCount]=Destination{};destinations[destCount++].anchor=p;found=true;break;}
        if(!found)break;
      }
      if(destCount!=2)continue;
      for(int i=0;i<2;++i){
        bool found=false;
        for(int n=0;n<N;++n){uint8_t p=uint8_t(random()%N);int distance=manhattan(p,port(i));
          if(!freeForBuilding(p)||!access(p)||distance<4||distance>14||(homeCount&&manhattan(p,homes[0].cell)<4))continue;
          homes[homeCount++]={p,0,0};found=true;break;}
        if(!found)break;
      }
      if(homeCount!=2)continue;
      bool crosses=(x(homes[0].cell)<10)!=(x(port(0))<10);
      crosses|=(x(homes[1].cell)<10)!=(x(port(1))<10);
      if(difficulty!=0&&!crosses)continue;
      placed=openingCost()<=stock-4;
    }
    if(!placed){ // Bounded, still randomized fallback with a verified short crossing.
      homeCount=destCount=2;int row=random()%4;
      homes[0]={cell(7,row+1),0,0};homes[1]={cell(2,row+5),0,0};
      destinations[0]=Destination{};destinations[0].anchor=cell(12,row);
      destinations[1]=Destination{};destinations[1].anchor=cell(6,row+4);
    }
    uint8_t firstColor=random()%3,secondColor=(firstColor+1+random()%2)%3;
    homes[0].color=destinations[0].color=firstColor;homes[1].color=destinations[1].color=secondColor;
    for(int i=0;i<2;++i){destinations[i].clock=random()%40;destinations[i].demand=1+random()%2;}
    cursor=homes[0].cell;
  }
  // A road-independent BFS used only to validate/generate an opening. It never
  // pre-builds roads or spends resources. Other buildings cannot be shortcuts.
  bool constructionPath(uint8_t start,uint8_t goal,int bridgeRow,uint8_t *parent) const {
    memset(parent,NONE,N);uint8_t queue[N];int head=0,tail=0;queue[tail++]=start;parent[start]=start;
    while(head<tail){uint8_t p=queue[head++];if(p==goal)return true;if(p!=start&&building(p))continue;
      for(int d=0;d<4;++d){uint8_t q=neighbor(p,d);if(q==NONE||parent[q]!=NONE||blocked(q))continue;
        if((water(p)||water(q))&&((d==0||d==2)||(water(q)&&y(q)!=bridgeRow)||(water(p)&&y(p)!=bridgeRow)))continue;
        parent[q]=p;queue[tail++]=q;}}
    return false;
  }
  uint16_t openingCost(int *bestRow=nullptr) const {
    uint16_t best=UINT16_MAX;uint8_t parent[N],draft[N];
    for(int row=0;row<H;++row){memset(draft,0,N);uint16_t cost=0;bool good=true;
      for(int i=0;i<2;++i){if(!constructionPath(homes[i].cell,port(i),row,parent)){good=false;break;}
        for(uint8_t p=port(i);p!=homes[i].cell;p=parent[p]){uint8_t q=parent[p];int d=direction(p,q);
          if(!(draft[p]&(1<<d))){++cost;draft[p]|=1<<d;draft[q]|=1<<opposite(d);}}}
      if(good&&cost<best){best=cost;if(bestRow)*bestRow=row;}}
    return best;
  }
  void cancel() { editing=false;planLength=0; }
  Result begin() {
    if(tool==ERASE)return erase(cursor);
    cursor=snap(cursor);if(blocked(cursor)||(tool==EXPRESS&&(!roads[cursor]||building(cursor)||water(cursor))))return BLOCKED;
    editing=true;planLength=1;plan[0]=cursor;return OK;
  }
  Result move(int d) {
    uint8_t next=neighbor(cursor,d);if(next==NONE)return BLOCKED;
    if(!editing||tool==EXPRESS){cursor=next;return OK;}
    if(planLength>1&&next==plan[planLength-2]){--planLength;cursor=next;return OK;}
    if(blocked(next)||(planLength>1&&building(cursor)))return BLOCKED;
    if((water(next)||water(cursor))&&(d==0||d==2))return BLOCKED;
    if(d>=4&&!diagonalClear(cursor,next,roads))return BLOCKED;
    for(int i=0;i<planLength;++i)if(plan[i]==next)return BLOCKED;
    if(planLength>=N)return TOO_LONG;
    if(d<4&&planLength>=2){uint8_t a=plan[planLength-2];int previous=direction(a,cursor);
      if(previous<4&&previous>=0&&previous%2!=d%2&&!roads[cursor]&&!water(a)&&!water(cursor)&&!water(next)&&!building(cursor)&&diagonalClear(a,next,roads)){
        plan[planLength-1]=next;cursor=next;return OK;
      }
    }
    plan[planLength++]=next;cursor=next;return OK;
  }
  Result cost(uint16_t &roadCost,uint8_t &bridgeCost) const {
    roadCost=0;bridgeCost=0;if(!editing||planLength<2)return BLOCKED;
    uint8_t draft[N];memcpy(draft,roads,N);
    for(int i=1;i<planLength;++i){uint8_t a=plan[i-1],b=plan[i];if(a>=N||b>=N)return BLOCKED;int d=direction(a,b);
      if(d<0||blocked(a)||blocked(b)||(i>1&&building(a))||((water(a)||water(b))&&(d==0||d==2))||!diagonalClear(a,b,draft))return BLOCKED;
      if(!(draft[a]&(1<<d))){++roadCost;if(water(a)&&!draft[a])++bridgeCost;if(water(b)&&!draft[b])++bridgeCost;
        draft[a]|=1<<d;draft[b]|=1<<opposite(d);
        if((water(a)&&!bridgeAxis(draft[a]))||(water(b)&&!bridgeAxis(draft[b])))return BLOCKED;}
    }
    return roadCost>stock?NO_ROADS:bridgeCost>bridges?NO_BRIDGE:OK;
  }
  Result commit() {
    if(!editing)return BLOCKED;
    if(tool==EXPRESS){uint8_t a=plan[0],b=cursor;
      if(a==b||!roads[b]||building(b)||water(b)||linked(a,b))return BLOCKED;
      if(!express||linkCount==LINKS)return NO_EXPRESS;
      links[linkCount++]={a,b};--express;cancel();return OK;
    }
    uint16_t r;uint8_t b;Result result=cost(r,b);if(result!=OK)return result;
    for(int i=1;i<planLength;++i)connect(plan[i-1],plan[i]);stock-=r;bridges-=b;cancel();return OK;
  }
  Result erase(uint8_t p) {
    p=snap(p);for(const auto &c:cars)if(c.active){uint8_t center=crossingFor(c.from,c.to);if(c.from==p||c.to==p||(center!=NONE&&footprint(center,p)))return BUSY;}
    if(!roads[p])return BLOCKED;
    for(int d=0;d<8;++d)if(roads[p]&(1<<d)){uint8_t q=neighbor(p,d);roads[q]&=~(1<<opposite(d));if(water(q)&&!roads[q]&&bridges<10)++bridges;++stock;}
    if(water(p)&&bridges<10)++bridges;roads[p]=0;junction[p]=0;
    for(int i=linkCount-1;i>=0;--i)if(links[i].a==p||links[i].b==p){links[i]=links[--linkCount];if(express<3)++express;}
    if(stock>400)stock=400;return OK;
  }
  bool crossAt(uint8_t a) const {
    return x(a)<W-1&&y(a)<H-1&&(roads[a]&(1<<5))&&(roads[a+1]&(1<<6));
  }
  uint8_t crossingFor(uint8_t a,uint8_t b) const {
    int dir=direction(a,b);if(dir<0||!(roads[a]&240)||!(roads[b]&240))return NONE;
    if(linked(a,b)||(dir<4&&(roads[a]&(1<<dir))))return NONE;
    for(int dy=-1;dy<=0;++dy)for(int dx=-1;dx<=0;++dx){int xx=x(a)+dx,yy=y(a)+dy;
      if(xx<0||yy<0||xx>=W-1||yy>=H-1)continue;uint8_t p=cell(xx,yy);
      if(crossAt(p)&&footprint(p,b))return p;
    }return NONE;
  }
  bool roadConnected(uint8_t a,uint8_t b) const {
    int d=direction(a,b);return d>=0&&((roads[a]&(1<<d))||crossingFor(a,b)!=NONE);
  }
  // BFS returns one hop; destination/home cells are endpoints, never shortcuts.
  uint8_t nextHop(uint8_t start,uint8_t goal) const {
    if(start>=N||goal>=N||start==goal)return NONE;
    uint8_t parent[N],queue[N];memset(parent,NONE,N);int head=0,tail=0;parent[start]=start;queue[tail++]=start;
    while(head<tail){uint8_t p=queue[head++];if(p==goal)break;if(p!=start&&building(p))continue;
      for(int d=0;d<8+linkCount;++d){uint8_t q=NONE;
        if(d<8){uint8_t n=neighbor(p,d);if(n!=NONE&&((roads[p]&(1<<d))||(d<4&&crossingFor(p,n)!=NONE)))q=n;}else {const auto &l=links[d-8];if(l.a==p)q=l.b;else if(l.b==p)q=l.a;}
        if(q!=NONE&&parent[q]==NONE){parent[q]=p;queue[tail++]=q;}
      }
    }
    if(parent[goal]==NONE)return NONE;uint8_t p=goal;while(parent[p]!=start)p=parent[p];return p;
  }
  bool occupied(uint8_t a,uint8_t b,const Car *self) const {
    for(const auto &c:cars)if(&c!=self&&c.active&&c.from==a&&c.to==b&&c.progress<192)return true;return false;
  }
  int homesOfColor(int color) const {
    int count=0;for(int i=0;i<homeCount;++i)count+=homes[i].color==color;return count;
  }
  int destinationsOfColor(int color,int skip=-1) const {
    int count=0;for(int i=0;i<destCount;++i)count+=i!=skip&&destinations[i].color==color;return count;
  }
  int rushColor() const {
    if(ticks<600)return -1;
    uint8_t colors[3];int count=0;for(int c=0;c<3;++c)if(destinationsOfColor(c))colors[count++]=c;
    return count?colors[((ticks-600)/400+homes[0].color)%count]:-1;
  }
  uint32_t geographicCapacity(int index) const {
    constexpr uint32_t SCALE=65536;
    int color=destinations[index].color;uint32_t capacity=0;
    // Share each home's two cars between matching destinations. Shorter trips
    // contribute more capacity; one distant house cannot slow an entire color.
    for(int h=0;h<homeCount;++h)if(homes[h].color==color){
      int trip=(manhattan(homes[h].cell,port(index))+1)*16+16;
      capacity+=2*SCALE/(destinationsOfColor(color)*trip);
    }
    return capacity?capacity:1;
  }
  uint16_t demandRate(int index) const {
    int utilization=difficulty==0?76:difficulty==2?98:90;
    int rush=rushColor();
    if(rush>=0)utilization+=destinations[index].color==rush?(difficulty==0?8:12):(difficulty==0?-6:-10);
    uint32_t capacity=geographicCapacity(index);
    int fleetFloor=int((6553600u+capacity*utilization-1)/(capacity*utilization));
    uint32_t age=week()-1;if(age>8)age=8;
    int rate=(difficulty==0?110:difficulty==2?64:80)-int(age)*8;
    int minimum=difficulty==0?50:difficulty==2?28:32;if(rate<minimum)rate=minimum;
    if(rate<fleetFloor)rate=fleetFloor;
    return uint16_t(rate);
  }
  bool siteAvailable(uint8_t p) const {
    if(p>=N||x(p)>=W-1||y(p)>=H-1)return false;
    if(!freeForBuilding(p)||!freeForBuilding(p+1)||!freeForBuilding(p+W)||!freeForBuilding(p+W+1))return false;
    for(int d=0;d<4;++d){uint8_t q=neighbor(p+W,d);
      if(q!=NONE&&q!=p&&q!=p+W+1&&!building(q)&&!water(q))return true;}
    return false;
  }
  bool startRenewal() {
    if(closing!=NONE||destCount<2)return false;
    int index=nextRenew%destCount;
    uint8_t anchor=destinations[index].anchor;
    // Try every lot, then reuse the old lot if the map has no spare space.
    int first=random()%N;
    for(int n=0;n<N;++n){uint8_t p=uint8_t((first+n)%N);if(siteAvailable(p)){anchor=p;break;}}
    int color=destinations[index].color;
    if(destinationsOfColor(color)>1){
      int best=-1,start=random()%3;
      for(int n=0;n<3;++n){int c=(start+n)%3,h=homesOfColor(c);if(!h||c==destinations[index].color)continue;
        if(best<0||destinationsOfColor(c,index)*homesOfColor(best)<destinationsOfColor(best,index)*h)best=c;}
      if(best>=0)color=best;
    }
    closing=uint8_t(index);replacement=anchor;replacementColor=uint8_t(color);closeClock=0;
    nextRenew=uint8_t((index+1)%destCount);
    destinations[index].demand=0;destinations[index].overload=0;destinations[index].clock=0;grace[index]=0;
    return true;
  }
  bool updateRenewal() {
    if(closing==NONE)return false;
    ++closeClock;bool occupied=false;
    for(auto &car:cars)if(car.active&&car.dest==closing)occupied=true;
    if(closeClock<CLOSE_NOTICE||(occupied&&closeClock<CLOSE_LIMIT))return false;
    // Give existing cars time to return. A disconnected vehicle is recalled
    // after 30 seconds so redevelopment cannot deadlock the city forever.
    for(auto &car:cars)if(car.active&&car.dest==closing)car.active=false;
    auto &d=destinations[closing];d=Destination{};d.anchor=replacement;d.color=replacementColor;
    grace[closing]=openingGrace();
    closing=replacement=NONE;replacementColor=0;closeClock=0;
    if(renewals<UINT32_MAX)++renewals;
    return true;
  }
  uint8_t step() {
    if(!alive||editing||rewardPending)return 0;
    uint8_t events=0;++ticks;
    for(auto &j:junction)if(j)--j;
    for(int i=0;i<destCount;++i){auto &d=destinations[i];if(d.flash)--d.flash;
      if(i==closing)continue;
      if(grace[i]){--grace[i];d.overload=0;continue;}
      if(++d.clock>=demandRate(i)){
        d.clock=0;int burst=ticks>=400&&random()%(difficulty==0?16:difficulty==2?8:10)==0?2:1;
        d.demand=uint8_t(d.demand+burst>9?9:d.demand+burst);
      }
      if(d.demand>=6){if(!d.overload)events|=WARNING;if(++d.overload>=overloadLimit()){alive=false;events|=FAILURE;}}
      else if(d.overload)--d.overload;
    }
    if(!alive)return events;
    for(auto &c:cars)if(c.active){
      if(c.hold){--c.hold;continue;}
      if(c.from!=c.to){int progress=int(c.progress)+(linked(c.from,c.to)?64:(direction(c.from,c.to)>=4||crossingFor(c.from,c.to)!=NONE)?23:32);
        uint8_t crossing=crossingFor(c.from,c.to);bool wait=false;
        if(crossing!=NONE&&c.progress<96&&progress>=96){for(const auto &other:cars)
          if(&other!=&c&&other.active&&other.from!=other.to&&other.progress>=96&&other.progress<160&&crossingFor(other.from,other.to)==crossing){wait=true;break;}}
        if(wait)continue;
        if(progress<256){c.progress=uint8_t(progress);continue;}c.from=c.to;c.progress=0;
        if(!c.returning&&c.from==port(c.dest)){auto &d=destinations[c.dest];if(d.demand){--d.demand;if(score<UINT32_MAX)++score;d.flash=14;events|=DELIVERY;}c.returning=true;c.hold=10;continue;}
        if(c.returning&&c.from==homes[c.home].cell){c.active=false;continue;}
      }
      uint8_t goal=c.returning?homes[c.home].cell:port(c.dest),next=nextHop(c.from,goal);
      if(next==NONE||junction[c.from]||occupied(c.from,next,&c)){c.to=c.from;continue;}
      c.to=next;int degree=__builtin_popcount(unsigned(roads[c.from]));junction[c.from]=degree>2?junctionDelay():1;
    }
    for(int n=0;n<homeCount;++n){int h=(n+ticks%homeCount)%homeCount;auto &home=homes[h];if(home.cooldown){--home.cooldown;continue;}int active=0;for(const auto &c:cars)if(c.active&&c.home==h)++active;if(active>=2)continue;
      int chosen=-1,mostNeed=0;
      for(int d=0;d<destCount;++d){if(d==closing||destinations[d].color!=home.color)continue;int onWay=0;for(const auto &c:cars)if(c.active&&!c.returning&&c.dest==d)++onWay;
        int need=int(destinations[d].demand)-onWay;
        if(need>mostNeed&&nextHop(home.cell,port(d))!=NONE){chosen=d;mostNeed=need;}
      }
      if(chosen>=0)for(auto &c:cars)if(!c.active){c=Car{};c.active=true;c.from=c.to=home.cell;c.home=h;c.dest=chosen;home.cooldown=20;break;}
    }
    if(updateRenewal())events|=OPENED;
    if(ticks%1200==0){rewardPending=true;events|=WEEK;}
    if(ticks%(difficulty==0?400:difficulty==2?300:360)==0&&growHome())events|=GROWTH;
    if(ticks%(difficulty==0?900:difficulty==2?600:700)==0&&growDestination())events|=GROWTH;
    if(ticks%3600==0&&startRenewal())events|=REDEVELOP;
    return events;
  }
  bool freeForBuilding(uint8_t p) const {
    if(p>=N||water(p)||building(p)||roads[p])return false;
    for(int d=4;d<8;++d){uint8_t a=neighbor(p,d-4),b=neighbor(p,(d-3)%4);if(a!=NONE&&b!=NONE){int edge=direction(a,b);if(edge>=4&&(roads[a]&(1<<edge)))return false;}}
    for(int d=0;d<4;++d){uint8_t q=neighbor(p,d);if(q!=NONE&&building(q))return false;}
    for(int i=0;i<linkCount;++i)if(links[i].a==p||links[i].b==p)return false;return true;
  }
  bool access(uint8_t p) const { for(int d=0;d<4;++d){uint8_t n=neighbor(p,d);if(n!=NONE&&!water(n)&&!building(n))return true;}return false; }
  bool growHome(int color=-1) {
    if(homeCount==HOMES)return false;
    if(color<0){
      int start=random()%3,best=-1;
      for(int n=0;n<3;++n){int c=(start+n)%3,d=destinationsOfColor(c);if(!d)continue;
        if(best<0||homesOfColor(c)*destinationsOfColor(best)<homesOfColor(best)*d)best=c;}
      color=best<0?0:best;
    }
    for(int tries=0;tries<200;++tries){uint8_t p=uint8_t(random()%N);if(!freeForBuilding(p)||!access(p))continue;
      homes[homeCount++]={p,uint8_t(color),0};return true;}return false;
  }
  bool growDestination() {
    if(destCount==DESTS)return false;uint8_t color=destCount%3;
    for(int c=0;c<3;++c)if(!destinationsOfColor(c)){color=c;break;}
    for(int tries=0;tries<300;++tries){uint8_t p=uint8_t(random()%N);if(x(p)>=W-1||y(p)>=H-1)continue;
      if(!freeForBuilding(p)||!freeForBuilding(p+1)||!freeForBuilding(p+W)||!freeForBuilding(p+W+1))continue;
      bool accessible=false;for(int d=0;d<4;++d){uint8_t q=neighbor(p+W,d);if(q!=NONE&&q!=p&&q!=p+W+1&&!building(q)&&!water(q))accessible=true;}if(!accessible)continue;
      bool hasHome=false;for(int i=0;i<homeCount;++i)hasHome|=homes[i].color==color;
      if(!hasHome&&!growHome(color))return false;
      // A newly grown matching home must not overlap the reserved footprint.
      if(!freeForBuilding(p)||!freeForBuilding(p+1)||!freeForBuilding(p+W)||!freeForBuilding(p+W+1))continue;
      destinations[destCount]=Destination{};destinations[destCount].anchor=p;destinations[destCount].color=color;grace[destCount++]=openingGrace();return true;
    }return false;
  }
  void reward(uint8_t choice) {
    if(!rewardPending||choice>2)return;
    stock+=choice==0?roadReward():kitRoadReward(choice==1);if(stock>400)stock=400;
    if(choice==1&&bridges<10)++bridges;
    if(choice==2&&express+linkCount<LINKS)++express;else if(choice==2){stock+=roadReward()-kitRoadReward(false);if(stock>400)stock=400;}
    rewardPending=false;
  }
  bool valid() const {
    if(!alive||!rng||difficulty>2||stock>400||bridges>10||express+linkCount>LINKS||homeCount<2||homeCount>HOMES||destCount<2||destCount>DESTS||linkCount>LINKS||cursor>=N||tool>EXPRESS)return false;
    if(nextRenew>=DESTS)return false;
    if(closing==NONE){if(replacement!=NONE||closeClock||replacementColor)return false;}
    else if(closing>=destCount||replacement>=N||x(replacement)>=W-1||y(replacement)>=H-1||replacementColor>2||closeClock>=CLOSE_LIMIT||!homesOfColor(replacementColor))return false;
    uint8_t used[N]{};
    for(int i=0;i<homeCount;++i){uint8_t p=homes[i].cell;if(p>=N||water(p)||used[p]||homes[i].color>2||homes[i].cooldown>20)return false;used[p]=1;}
    for(int i=0;i<destCount;++i){auto &d=destinations[i];uint8_t p=d.anchor;if(p>=N||x(p)>=W-1||y(p)>=H-1||d.color>2||d.demand>9||d.clock>=3600||d.overload>=overloadLimit()||d.flash>14||grace[i]>OPEN_GRACE||!homesOfColor(d.color))return false;
      if(i==closing&&(d.demand||d.clock||d.overload||grace[i]))return false;
      for(int o:{0,1,W,W+1}){uint8_t q=p+o;if(water(q)||used[q])return false;used[q]=1;}}
    if(closing!=NONE&&replacement!=destinations[closing].anchor){
      for(int o:{0,1,W,W+1}){uint8_t q=replacement+o;if(water(q)||used[q])return false;used[q]=1;}}
    for(int p=0;p<N;++p){if(junction[p]>14||(blocked(p)&&roads[p])||(water(p)&&!bridgeAxis(roads[p])))return false;
      for(int d=0;d<8;++d)if(roads[p]&(1<<d)){uint8_t q=neighbor(p,d);if(q==NONE||!(roads[q]&(1<<opposite(d)))||!diagonalClear(p,q,roads))return false;}}
    for(int i=0;i<linkCount;++i){auto &l=links[i];if(l.a>=N||l.b>=N||l.a==l.b||!roads[l.a]||!roads[l.b]||building(l.a)||building(l.b)||water(l.a)||water(l.b))return false;
      for(int j=0;j<i;++j)if((links[j].a==l.a&&links[j].b==l.b)||(links[j].a==l.b&&links[j].b==l.a))return false;}
    for(auto &c:cars)if(c.active){if(c.from>=N||c.to>=N||c.home>=homeCount||c.dest>=destCount||c.hold>10||homes[c.home].color!=destinations[c.dest].color)return false;
      if(!roads[c.from]||!roads[c.to]||blocked(c.from)||blocked(c.to)||(c.from==c.to&&c.progress))return false;
      if(c.from!=c.to){int d=direction(c.from,c.to);if(!linked(c.from,c.to)&&!roadConnected(c.from,c.to))return false;}}
    return true;
  }
};
