#include<iostream>
#include "LineSegmentIntersector.h"

using namespace std;

#define DELTA 0.00005

void swap(LineSegmentIntersector::LSIPoint& p1, LineSegmentIntersector::LSIPoint& p2){
    LineSegmentIntersector::LSIPoint temp(p1, p1.lsiSegment, p1.et);

    // p1 = p2
    p1.lsiSegment = p2.lsiSegment;
    p1 = p2;
    p1.et = p2.et;

    // p2 = temp
    p2.lsiSegment = temp.lsiSegment;
    p2 = temp;
    p2.et = temp.et;
}

Point LineSegmentIntersector::lastReference = NAN_POINT;

LineSegmentIntersector::LSISegment& LineSegmentIntersector::LSISegment::operator=(LineSegmentIntersector::LSISegment& a) {
    if(a.start_pt().is_nan() || a.end_pt().is_nan()) return(a);
    // this->ls = a.ls;
    this->start_point = a.start_pt();
    this->end_point = a.end_pt();
    return *this;
}

LineSegmentIntersector::LSIPoint::LSIPoint( Point p,
                                            LineSegmentIntersector::LSISegment lsis,
                                            LineSegmentIntersector::EventType et
                                           ){
    this -> x = p.x;
    this -> y = p.y;
    this->lsiSegment = lsis;
    this->et = et;
}

bool LineSegmentIntersector::LSIPoint::operator<(LineSegmentIntersector::LSIPoint lsip2) {
    Point p1 = *this;
    Point p2 = lsip2;

    if (p1.y < p2.y){
        return false;
    }
    else if ( abs(p1.y - p2.y)<ERROR ){
        return p1.x < p2.x;
    }
    else{
        return true;
    }
}

LineSegmentIntersector::LSIPoint& LineSegmentIntersector::LSIPoint::operator= (LSIPoint& lp){
    this -> x = lp.x;
    this -> y = lp.y;
    this -> lsiSegment = lp.lsiSegment;
    this -> et = lp.et;
}

LineSegmentIntersector::LSISegment::LSISegment(){

}

LineSegmentIntersector::LSISegment::LSISegment(LineSegment l){
    this -> start_point = l.start_pt();
    this -> end_point = l.end_pt();
}

bool LineSegmentIntersector::LSISegment::operator<(const LineSegmentIntersector::LSISegment& lsi) const {
    //cout << *this << " < " << lsi << " @ " << lastReference << " : ";
    Point this_upper = this->start_pt(), this_lower = this->end_pt();
    Point lsi_upper = lsi.start_pt(), lsi_lower = lsi.end_pt();

    const LSISegment debug = LSISegment(LineSegment(Point(1,9),Point(3,8)));

    if((this_upper.y < this_lower.y) || ( abs(this_upper.y - this_lower.y)<ERROR && this_upper.x >= this_lower.x)){
        Point temp = this_upper;
        this_upper = this_lower;
        this_lower = temp;
    }
    if((lsi_upper.y < lsi_lower.y) || (abs(lsi_upper.y - lsi_lower.y)<ERROR && lsi_upper.x >= lsi_lower.x)){
        Point temp = lsi_upper;
        lsi_upper = lsi_lower;
        lsi_lower = temp;
    }

    if ((lastReference == this_upper || lastReference == this_lower) &&
        (lastReference == lsi_upper  || lastReference == lsi_lower) ){
        // CLEANUP DEBUG COMMENT : SHOULDNT GO HERE
        if (lastReference == this_upper && lastReference == lsi_upper){
            Point comp = Point(lastReference.x, lastReference.y - DELTA);

            bool toReturn = this->horizontal_projection(comp).x
                   < lsi.horizontal_projection(comp).x;
            //cout << "1."<< toReturn << "\n";
            return toReturn;
        }
        if (lastReference == this_upper && lastReference == lsi_lower){
//            cout << true << "\n";
            bool toReturn = true;
            //cout << "2."<< toReturn << "\n";

            return true;
        }
        if (lastReference == this_lower && lastReference == lsi_lower){
//            cout << "LL\n";
            Point comp = Point(lastReference.x, lastReference.y + DELTA);

            bool toReturn = this->horizontal_projection(comp).x
                   < lsi.horizontal_projection(comp).x;
//            cout << toReturn << "\n";
         //   cout << "3."<< toReturn << "\n";

            return toReturn;
        }
        if (lastReference == this_lower && lastReference == lsi_upper){
//            cout << false << "\n";
        //    cout << "4."<< false << "\n";

            return false;
        }

    }
    else if(lastReference == this_lower || lastReference == lsi_lower){
        if (lastReference == Point(1,6)){ //this_lower == Point(1,6) || lsi_lower == Point(1,6)){
            //cout << *this << " < " << lsi << "@ " << lastReference <<" = ";
        }
        Point comp;
        if(this->contains_point(lastReference) && lsi.contains_point(lastReference)){
            comp = Point(lastReference.x, lastReference.y + DELTA);
        }
        else {
            comp = lastReference;
        }

        bool toReturn = this->horizontal_projection(comp).x
               < lsi.horizontal_projection(comp).x;
        //if (lastReference == Point(1,6)){
        //    cout << toReturn << "\n";
        //}
//        cout << "5."<< toReturn << "\n";

        return toReturn;
    }
    else if(lastReference == this_upper || lastReference == lsi_upper){
        Point comp;
        if(this->contains_point(lastReference) && lsi.contains_point(lastReference)){
            comp = Point(lastReference.x, lastReference.y - DELTA);
        //    cout << "A" ;
        }
        else {
            comp = lastReference;
        //    cout << "B";

        }
        bool toReturn = this->horizontal_projection(comp).x
                        < lsi.horizontal_projection(comp).x;
//        cout << "6."<< toReturn;// << "\n";
//        cout <<"\t"<<this->horizontal_projection(comp).x << " < "
//             << lsi.horizontal_projection(comp).x << "\n";
        return toReturn;
    }
    else  {
        // Only in case of ordering event point
        Point comp = Point(lastReference.x, lastReference.y - DELTA);
        bool toReturn = this->horizontal_projection(comp).x
                        < lsi.horizontal_projection(comp).x;
        //cout << "7."<< toReturn << "\n";

        return toReturn;
    }
}

bool LineSegmentIntersector::LSISegment::operator>(const LineSegmentIntersector::LSISegment& lsi) const {
     return !(*this < lsi) && (lsi < *this);
}

bool LineSegmentIntersector::LSISegment::operator !=(const LineSegmentIntersector::LSISegment& lsi) const {
    return *this < lsi || lsi < *this;
}

bool LineSegmentIntersector::LSISegment::operator <=(const LineSegmentIntersector::LSISegment& lsi) const {
    return !(lsi < *this);
}

LineSegmentIntersector::LineSegmentIntersector(std::vector<LineSegment> &i, LSIGraphix& l) 
                                : input(i), lsiGfx(l){
    lsiGfx.init_lines(i);
    for (auto it : input){
        LSISegment lsi_it(it);
        Point p1 = it.start_pt();
        Point p2 = it.end_pt();
        //cout << eventQueue.size();
        EventType p1Type, p2Type;

        if(p1.y > p2.y ){
            // P1 is upper;
            p1Type = UPPER;
            p2Type = LOWER;
        }
        else if( abs(p1.y - p2.y)<ERROR ) {
            if(p1.x < p2.x){
                // P1 is upper
                p1Type = UPPER;
                p2Type = LOWER;
            }
            else{
                //P2 is upper
                p1Type = LOWER;
                p2Type = UPPER;
            }
        }
        else{
            // P2 is upper
            p1Type = LOWER;
            p2Type = UPPER;
        }

        LSIPoint lsip1(p1,lsi_it,p1Type);
        LSIPoint lsip2(p2,lsi_it,p2Type);

        eventQueue.insert(lsip1);
        eventQueue.insert(lsip2);
    }

    LSIPoint pt= eventQueue.peek();
    lastReference.x = pt.x;
    lastReference.y = pt.y;

}

LSIResult LineSegmentIntersector::computeIntersections() {
    while(eventQueue.size() > 0){
        upper.clear();
        lower.clear();
        containing.clear();

        //std::cout << "event queue size = " << eventQueue.size() << std::endl;

        LSIPoint pt = eventQueue.peek();
        Point curr;
        curr.x = pt.x; curr.y = pt.y;

        lsiGfx.update_event(curr);
        handleEventPoint(curr);
        lsiGfx.render();
    }

    return result;

}

void LineSegmentIntersector::handleEventPoint(Point curr) {
    cout << curr << "\n";
    //cout<<"EventQueue min="<<eventQueue.peek()<<" curr="<<curr<<endl;
    lastReference = curr;
    while(eventQueue.size() > 0 && eventQueue.peek() == curr) {
        LSIPoint toInsert = eventQueue.extractMin();
        //cout<<"Next min:"<<eventQueue.peek()<<" toInsert="<<toInsert<<endl;
        if (toInsert.et == UPPER) {
            upper.insert(toInsert.lsiSegment);
            cout << "\t" << toInsert.lsiSegment;
            //lastContaining = &(toInsert.lsiSegment);
            cout << "UPPER SIZE = " << upper.size() << "\n";
        } else if (toInsert.et == LOWER) {
            lower.insert(toInsert.lsiSegment);
            cout << "\t" << toInsert.lsiSegment;
            //lastContaining = &(toInsert.lsiSegment);
            cout << "LOWER SIZE = " << lower.size() << "\n";
        } else if (toInsert.et == CONTAINING) {
            LineSegment candidate = toInsert.lsiSegment;
            if(curr != candidate.start_pt() && curr != candidate.end_pt()) {
                containing.insert(toInsert.lsiSegment);
                cout << "\t" << toInsert.lsiSegment;
//                lastContaining = &(toInsert.lsiSegment);
                cout << "CONTAINING SIZE = " << containing.size() << "\n";
            }
        } else {
            std::cout << "error\n";
        }
        //cout<<eventQueue.peek()<<"=="<<curr<<" "<<(eventQueue.peek()==curr)<<endl;
    }

    // For reordering efficiently
    //lastReference = temp;
    for (auto i : lower){
        status.remove(i);
        cout << "\tRemoving : " << i << "\n";
    }

    for (auto i : containing){
        status.remove(i);
        cout << "\tContating Removing : " << i << "\n";
    }

    //lastReference = curr;
    set<LSISegment> luc;

    for (auto i : upper) {
        luc.insert(i);
    }

    for (auto i : lower) {
        luc.insert(i);
    }

    for (auto i : containing) {
        luc.insert(i);
    }

    if(luc.size() > 1){
        for (auto i : luc){
            result[curr].push_front(i);
        }
    }

    // Ordering New parameter del line
    lastReference = curr;

    set<LSISegment> uc;
    for (auto i : upper){
        uc.insert(i);
        cout << "\tInserting : " << i << "\n";
        status.insert(i);
    }
    for (auto i : containing){
        uc.insert(i);
        cout << "\tReinserting : " << i << "\n";
        status.insert(i);
    }

    cout << "\tStatus : "; // size = " << status.size();
    status.inorder();
    cout << "\n\t===================================\n";


    if (uc.size() == 0){
        // Fake vertical line insert in status;
        //cout << "Test1\n";
        // CHANGE 1
        Point fakeLineLower = Point(curr.x, curr.y + DELTA);
        LineSegment fakeLineSegment(curr,fakeLineLower);
        LSISegment fakeLSISegment(fakeLineSegment);
        // Get left and right neighbors;
        status.insert(fakeLSISegment);
        LSISegment* sl = status.searchL(fakeLSISegment);
        LSISegment* sr = status.searchR(fakeLSISegment);

        status.remove(fakeLSISegment);

        findNewEvent(sl,sr,curr);
    }
    else{
        LSISegment sl = *(uc.begin());
        LSISegment sr = *(--uc.end());

        LSISegment* sll = status.searchL(sl);
        LSISegment* srr = status.searchR(sr);

        findNewEvent(sll,&sl,curr);
        findNewEvent(srr,&sr,curr);

    }
}

void LineSegmentIntersector::findNewEvent(LSISegment* sl, LSISegment* sr, Point pt){
    if(sl == NULL || sr == NULL) return;
    else{
        Point intersectionPoint = sl->intersects_at(*sr);
        if (intersectionPoint.is_nan()) return;
        else{
            if( intersectionPoint.y < pt.y ||
                ( abs(intersectionPoint.y - pt.y)<ERROR && intersectionPoint.x > pt.x)){
                LSIPoint toAdd(intersectionPoint, *sl, LineSegmentIntersector::CONTAINING);
                LSIPoint toAdd2(intersectionPoint, *sr, LineSegmentIntersector::CONTAINING);
                //cout<<"FNE:"<<toAdd<<" "<<toAdd2<<" intersectionPoint="<<intersectionPoint<<endl;
                //cout<<"Old size="<<eventQueue.size()<<" ";
                eventQueue.insert(toAdd);
                eventQueue.insert(toAdd2);
                //cout<<"New size="<<eventQueue.size()<<endl;
            }
        }
    }
}