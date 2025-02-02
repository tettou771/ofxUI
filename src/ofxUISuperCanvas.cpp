/**********************************************************************************
 
 Copyright (C) 2012 Syed Reza Ali (www.syedrezaali.com)
 
 Permission is hereby granted, free of charge, to any person obtaining a copy of
 this software and associated documentation files (the "Software"), to deal in
 the Software without restriction, including without limitation the rights to
 use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 of the Software, and to permit persons to whom the Software is furnished to do
 so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 
 **********************************************************************************/

#include "ofxUISuperCanvas.h"
#include "ofxUI.h"


ofxUISuperCanvas::ofxUISuperCanvas(const ofxUISuperCanvas &other)
: size(other.size),
title(other.title),
hitPoint(other.hitPoint),
deltaTime(other.deltaTime),
lastHitTime(other.lastHitTime),
bIsMinified(other.bIsMinified),
bTitleLabelHit(other.bTitleLabelHit)
{
    if (other.canvasTitle) {
        canvasTitle = new ofxUILabel(*other.canvasTitle);
        headerWidgets.push_back(canvasTitle);
        addWidgetPosition(canvasTitle, widgetPosition, widgetAlign);
    }
    else {
        canvasTitle = NULL;
    }
}

ofxUISuperCanvas::ofxUISuperCanvas(string _label, ofxUIRectangle r, int _size) : ofxUICanvas(r)
{
    superInit(_label, _size);
}

ofxUISuperCanvas::ofxUISuperCanvas(string _label, float x, float y, float w, float h, int _size) : ofxUICanvas(x, y, w, h)
{
    superInit(_label, _size);
}

ofxUISuperCanvas::ofxUISuperCanvas(string _label, float x, float y, float w, float h, ofxUICanvas *sharedResources, int _size) : ofxUICanvas(x, y, w, h, sharedResources)
{
    superInit(_label, _size);
}

ofxUISuperCanvas::ofxUISuperCanvas(string _label, int _size) : ofxUICanvas()
{
    superInit(_label, _size);
}

ofxUISuperCanvas::ofxUISuperCanvas(string _label, ofxUICanvas *sharedResources, int _size) : ofxUICanvas(sharedResources)
{
    superInit(_label, _size);
}

void ofxUISuperCanvas::superInit(string _label, int _size)
{
    size = _size;
    title = _label;
    kind = OFX_UI_WIDGET_SUPERCANVAS;
    canvasTitle = new ofxUILabel(rect->getWidth()-widgetSpacing*2, title, size);
    canvasTitle->setEmbedded(true);
    headerWidgets.push_back(canvasTitle);
    addWidgetPosition(canvasTitle, widgetPosition, widgetAlign);
    deltaTime = .35;
    lastHitTime = ofGetElapsedTimef();
    bIsMinified = false;
    lastHitTime = 0;
    bTitleLabelHit = false;
    hitPoint = ofxUIVec2f(0.0, 0.0);
}

void ofxUISuperCanvas::setDeltaTime(float _deltaTime)
{
    deltaTime = _deltaTime;
}

void ofxUISuperCanvas::setMinified(bool _bIsMinified)
{
    if(bIsMinified != _bIsMinified)
    {
        bIsMinified = _bIsMinified;
        if(bIsMinified)
        {
            minify();
        }
        else
        {
            maximize();
        }
    }
}

bool ofxUISuperCanvas::isMinified()
{
    return bIsMinified;
}

void ofxUISuperCanvas::toggleMinified()
{
    setMinified(!bIsMinified);
}

ofxUILabel *ofxUISuperCanvas::getCanvasTitle()
{
    return canvasTitle;
}

void ofxUISuperCanvas::autoSizeToFitWidgets()
{
    ofxUICanvas::autoSizeToFitWidgets();
    canvasTitle->getRect()->setWidth(rect->getWidth()-widgetSpacing*2);    
}

bool ofxUISuperCanvas::didHitHeaderWidgets(float x, float y)
{
    vector<ofxUIWidget *>::iterator it = headerWidgets.begin();
    vector<ofxUIWidget *>::iterator eit = headerWidgets.end();
    for(; it != eit; ++it)
    {
        if((*it)->isHit(x, y))
        {
            return true;
        }
    }
    return false;
}

void ofxUISuperCanvas::keyPressed(int key)
{
    if(getIsBindedToKey(key) && !bKeyHit)
    {
        bKeyHit = true;
        lastPosition = ofxUIVec2f(rect->getX(), rect->getY());
        setMinified(false);
        rect->setX(ofGetMouseX());
        rect->setY(ofGetMouseY()); 
        if(getTriggerType() & OFX_UI_TRIGGER_BEGIN)
        {
            triggerEvent(this);
        }
    }

    ofxUICanvas::keyPressed(key);
}

void ofxUISuperCanvas::keyReleased(int key)
{
    if(getIsBindedToKey(key) && bKeyHit)
    {
        bKeyHit = false;
        if((ofGetElapsedTimef() - lastHitTime) < deltaTime)
        {
            setMinified(false);
            lastPosition = ofxUIVec2f(ofGetMouseX(), ofGetMouseY());
            if(getTriggerType() & OFX_UI_TRIGGER_BEGIN)
            {
                triggerEvent(this);
            }
        }
        else
        {
            setMinified(true);
            rect->setX(lastPosition.x);
            rect->setY(lastPosition.y);
            if(getTriggerType() & OFX_UI_TRIGGER_END)
            {
                triggerEvent(this);
            }
        }
        lastHitTime = ofGetElapsedTimef();
    }
    ofxUICanvas::keyReleased(key);
}

#ifdef OFX_UI_TARGET_TOUCH

void ofxUISuperCanvas::touchDown(float x, float y, int id)
{
    if(touchId == -1)
    {
        if(rect->inside(x, y) && didHitHeaderWidgets(x, y))
        {
            touchId = id;
            bTitleLabelHit = true;
            hitPoint.set(x - rect->getX(), y - rect->getY());
            return;
        }
    }
    canvasTouchDown(x, y, id);
}

void ofxUISuperCanvas::touchMoved(float x, float y, int id)
{
    if(touchId == id)
    {
        if(bTitleLabelHit)
        {
            rect->setX(x - hitPoint.x);
            rect->setY(y - hitPoint.y);
            return;
        }
    }
    canvasTouchMoved(x, y, id);
}

void ofxUISuperCanvas::touchUp(float x, float y, int id)
{
    if(touchId == id)
    {
        touchId = -1;
        bTitleLabelHit = false;
    }
    canvasTouchUp(x, y, id);
}

void ofxUISuperCanvas::touchDoubleTap(float x, float y, int id)
{
    if(rect->inside(x, y) && didHitHeaderWidgets(x, y))
    {
        if(isMinified())
        {
            setMinified(false);
            if(getTriggerType() & OFX_UI_TRIGGER_BEGIN)
            {
                triggerEvent(this);
            }
        }
        else
        {
            setMinified(true);
            if(getTriggerType() & OFX_UI_TRIGGER_END)
            {
                triggerEvent(this);
            }
        }
        return;
    }
    canvasTouchDoubleTap(x, y, id);
}

void ofxUISuperCanvas::touchCancelled(float x, float y, int id)
{
    if(touchId == id)
    {
        touchId = -1;
        bTitleLabelHit = false;
    }
    canvasTouchCancelled(x, y, id);
}

#else

void ofxUISuperCanvas::onMouseReleased(ofMouseEventArgs& data)
{
    bTitleLabelHit = false;
    mouseReleased(data.x, data.y, data.button);
}

void ofxUISuperCanvas::onMousePressed(ofMouseEventArgs& data)
{
    if(rect->inside(data.x, data.y) && didHitHeaderWidgets(data.x, data.y))
    {
        bTitleLabelHit = true;
        hitPoint.set(data.x - rect->getX(), data.y - rect->getY());
        
        if((ofGetElapsedTimef() - lastHitTime) < deltaTime)
        {
            if(isMinified())
            {
                setMinified(false);
                if(getTriggerType() & OFX_UI_TRIGGER_BEGIN)
                {
                    triggerEvent(this);
                }
            }
            else
            {
                setMinified(true);
                if(getTriggerType() & OFX_UI_TRIGGER_END)
                {
                    triggerEvent(this);
                }
            }
            return;
        }
        lastHitTime = ofGetElapsedTimef();
    }
    mousePressed(data.x, data.y, data.button);
}

void ofxUISuperCanvas::onMouseDragged(ofMouseEventArgs& data)
{
    if(bTitleLabelHit)
    {
        rect->setX(data.x - hitPoint.x);
        rect->setY(data.y - hitPoint.y);
        return;
    }
    mouseDragged(data.x, data.y, data.button);
}

#endif

#ifndef OFX_UI_NO_XML

void ofxUISuperCanvas::saveSettings(string fileName)
{
    ofXml XML;

	auto CanvasTag = XML.appendChild("Canvas");
	{
		CanvasTag.appendChild("Kind").set(getKind());
		CanvasTag.appendChild("Name").set(getName());
		CanvasTag.appendChild("IsMinified").set(bIsMinified ? 1 : 0);
		CanvasTag.appendChild("XPosition").set(rect->getX());
		CanvasTag.appendChild("YPosition").set(rect->getY());
	}
	
    for(int i = 0; i < widgetsWithState.size(); i++)
    {
		auto WidgetTag = XML.appendChild("Widget");
		{
			WidgetTag.appendChild("Kind").set(widgetsWithState[i]->getKind());
			WidgetTag.appendChild("Name").set(widgetsWithState[i]->getName());
			widgetsWithState[i]->saveState(WidgetTag);
		}
    }
    XML.save(fileName);
}

void ofxUISuperCanvas::loadSettings(string fileName)
{
	ofXml XML;
	XML.load(fileName);
	if (XML) {
		auto WidgetTags = XML.getChildren("Widget");
		for (auto &WidgetTag : WidgetTags) {
			string name = WidgetTag.getChild("Name").getValue();
			ofxUIWidget *widget = getWidget(name);
			if (widget != NULL && widget->hasState()) {
				widget->loadState(WidgetTag);
				if (bTriggerWidgetsUponLoad) {
					triggerEvent(widget);
				}
			}
		}
	}

	auto CanvasTag = XML.getChild("Canvas");
	if (CanvasTag) {
		int value = CanvasTag.getChild("IsMinified").getBoolValue();
		setMinified((value ? 1 : 0));
		rect->setX(CanvasTag.getChild("XPosition").getFloatValue());
		rect->setY(CanvasTag.getChild("YPosition").getFloatValue());
	}

    hasKeyBoard = false;
}

#endif

void ofxUISuperCanvas::addWidgetToHeader(ofxUIWidget *widget)
{
    if(canvasTitle != NULL)
    {
        float y = widget->getRect()->getY();
        float h = widget->getRect()->getHeight();
        float th = canvasTitle->getRect()->getHeight();
        float dh = (h-th)*0.5;
        canvasTitle->getRect()->setY(y + dh);
    }
    headerWidgets.push_back(widget);
}

void ofxUISuperCanvas::removeWidgets()
{
    ofxUICanvas::removeWidgets();
    headerWidgets.clear();
    resetPlacer(); 
    canvasTitle = new ofxUILabel(rect->getWidth()-widgetSpacing*2, title, size);
    canvasTitle->setEmbedded(true);
    headerWidgets.push_back(canvasTitle);
    addWidgetPosition(canvasTitle, OFX_UI_WIDGET_POSITION_DOWN, OFX_UI_ALIGN_FREE);
}

void ofxUISuperCanvas::minify()
{
    for(vector<ofxUIWidget *>::iterator it = widgets.begin(); it != widgets.end(); ++it)
    {
        ofxUIWidget *w = (*it);
        if(w != NULL)
        {
            w->setVisible(false);
        }
    }
    for(vector<ofxUIWidget *>::iterator it = headerWidgets.begin(); it != headerWidgets.end(); ++it)
    {
        ofxUIWidget *w = (*it);
        if(w != NULL)
        {
            w->setVisible(true);
        }
    }
    autoSizeToFitWidgets();
}

void ofxUISuperCanvas::maximize()
{
    for(vector<ofxUIWidget *>::iterator it = widgets.begin(); it != widgets.end(); ++it)
    {
        ofxUIWidget *w = (*it);
        if(w != NULL)
        {
            w->setVisible(true);
        }
    }
    autoSizeToFitWidgets();
}
