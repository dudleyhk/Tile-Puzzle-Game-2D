/*



https://blackboard.uwe.ac.uk/bbcswebdav/pid-4634231-dt-content-rid-8695402_2/courses/UFCFWA-30-1_15sep_1/Worksheet%2013%283%29.pdf

Sprite Guide
http://cocos2d-x.org/documentation/programmers-guide/3/index.html
*/

#include "GameScene.h"

cocos2d::Scene * GameScene::createScene()
{
	// 'scene' & 'layer' are autoreleased objects
	cocos2d::Scene* playScene = cocos2d::Scene::create();
	auto playLayer = GameScene::create();

	// add layer as a child to a scene
playScene->addChild(playLayer);
playLayer->initLayer();

// return the scene
return playScene;
}


/* CREATE_FUNC(GameScene) --- create scene with parameters
GameScene* GameScene::create(int hi)
{
	GameScene *pRet = new(std::nothrow) GameScene();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}
*/

GameScene::GameScene()
{
	; // Empty
}

GameScene::~GameScene()
{
	; // Empty
}

bool GameScene::initLayer()
{
	if (!Layer::init())
	{
		return false;
	}

	addPuzzleBoard();
	addEvent();

	return true;
}

void GameScene::addPuzzleBoard()
{
	puzzleBoard->createImage();
	puzzleBoard->getSpriteList(&tileList);
	puzzleBoard->getDirection();

	// Display puzzle.
	for (unsigned int index = 0; index < tileList.size(); index++)
	{
		// display
		this->addChild(tileList.at(index));
	}
}

void GameScene::addEvent()
{
	// create a mouse listener and hook into our event callback function
	auto clickListener = cocos2d::EventListenerTouchOneByOne::create();

	clickListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);

	// register event listener to receive events. 
	_eventDispatcher->addEventListenerWithSceneGraphPriority(clickListener, this);
}

bool GameScene::onTouchBegan(cocos2d::Touch* click, cocos2d::Event* event)
{
	// Get info on where the screen has been clicked
	cocos2d::Point point = click->getLocationInView();

	// convert these coordinates into world coordinates
	point = cocos2d::Director::getInstance()->convertToGL(point);

	// run through the tile list
	for (unsigned int heightIndex = 0; heightIndex < 4; heightIndex++)
	{
		for (unsigned int widthIndex = 0; widthIndex < 4; widthIndex++)
		{
			//if any sprite contains the point coordinates.
			if (tileList.at((4 * heightIndex) + widthIndex)->
				getBoundingBox().containsPoint(point))
			{
				checkForEmpty((4 * heightIndex) + widthIndex); 

				return true;
			}
		}
	}
	return false;
}

void GameScene::checkForEmpty(int tileID) // this is currently the same as posID
{
	int n_posID = -1;
	int posID = tileList.at(tileID)->getPositionID();

	//save for variable as global varibale   //this may not ned to be done /////////////////////CHHHECK
	hIndex = posID / 4;
	wIndex = posID - (4 * hIndex);

	checkLeft(posID - 1, &n_posID);
	if (n_posID == -1)
	{
		checkRight(posID + 1, &n_posID);
		if (n_posID == -1)
		{
			checkUp(posID + 4 /* 4 = number of seg width*/, &n_posID);
			if (n_posID == -1)
			{
				checkDown(posID - 4 /* 4 = number of seg width*/, &n_posID);
				if (n_posID == -1)
				{
					; // empty tile not found, player cannot move this tile
				}
			}
		}
	}
	if (n_posID != -1)
	{
		swapTiles(tileID);
	}
}

void GameScene::checkLeft(int _posID, int* const  n_posID)
{
	*n_posID = _posID;

	// check 
	// if either of these functions return false
	if (!(puzzleBoard->checkInBounds(hIndex, (wIndex - 1)) && getEmptyTilePos(*n_posID)))
	{
		// set newPos to an unused value
		*n_posID = -1;
	}
}

void GameScene::checkRight(int _posID, int* const n_posID)
{
	*n_posID = _posID;

	// if either of these functions return false
	if (!(puzzleBoard->checkInBounds(hIndex, (wIndex + 1)) && getEmptyTilePos(_posID)))
	{
		// set newPos to an unused value
		*n_posID = -1;
	}
}

void GameScene::checkUp(int _posID, int* const n_posID)
{
	*n_posID = _posID;

	// if either of these functions return false
	if (!(puzzleBoard->checkInBounds((hIndex + 1), wIndex) && getEmptyTilePos(_posID)))
	{
		// set newPos to an unused value
		*n_posID = -1;
	}
}

void GameScene::checkDown(int _posID, int* const n_posID)
{
	*n_posID = _posID;

	// if either of these functions return false
	if (!(puzzleBoard->checkInBounds((hIndex - 1), wIndex) && getEmptyTilePos(_posID)))
	{
		// set newPos to an unused value
		*n_posID = -1;
	}
}

bool GameScene::getEmptyTilePos(int _posID)
{
	if (_posID == tileList.at(empTileID)->getPositionID())
	{
		return true;
	}
	return false;
}

void GameScene::swapTiles(int tileID)
{
	//selected sprite
	auto selectedSprite = tileList.at(tileID);
	cocos2d::Vec2 selectedSpritePosition = tileList.at(tileID)->getPosition();

	// empty sprite
	auto emptySprite = tileList.at(empTileID);
	cocos2d::Vec2 emptySpritePosition = tileList.at(empTileID)->getPosition();


	auto moveEmptySprite = cocos2d::MoveTo::create(0.1f, cocos2d::Vec2(
		selectedSpritePosition.x, 
		selectedSpritePosition.y));
	auto moveSelectedSprite = cocos2d::MoveTo::create(0.1f, cocos2d::Vec2(
		emptySpritePosition.x,
		emptySpritePosition.y));


	if (selectedSprite->getNumberOfRunningActions() == 0 && 
		emptySprite->getNumberOfRunningActions() == 0)
	{
		int temp = tileList.at(tileID)->getPositionID();

		selectedSprite->runAction(moveSelectedSprite);
		emptySprite->runAction(moveEmptySprite);

		// set the position value as new positions
		tileList.at(tileID)->setPositionID(tileList.at(empTileID)->getPositionID());

		// set the tile positions as new positions
		tileList.at(empTileID)->setPositionID(temp);
	}

	// track amount of moves taken.
	numOfMoves++;

	cocos2d::log("%i", numOfMoves);

	// check to see if the board is complete at this point
	if (boardComplete())
	{
		cocos2d::log("gameWon");

		// at this point bring up a message would u like to replay or main menu?
	}
}


// end state pseudocode -- call before every move -- this is so at the very beginning of the game the board can check if the board is set to its original position

bool GameScene::boardComplete()
{
	int rightPlace = 0;

	puzzleBoard->getCorrectTiles(&rightPlace);

	if (rightPlace == tileList.size())
	{
		return true;
	}

	return false;
}
