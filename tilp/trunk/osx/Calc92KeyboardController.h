/*  TiLP - Linking program for TI calculators
 *  Copyright (C) 2001-2002 Julien BLACHE <jb@technologeek.org>
 *
 *  Cocoa GUI for Mac OS X
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#import <Cocoa/Cocoa.h>

#include "cocoa_structs.h"

@interface Calc92KeyboardController : NSObject
{    
    IBOutlet id keyboardWindow;
    IBOutlet id skinView;
    IBOutlet id textArea;
    IBOutlet id scrollView;
    
    IBOutlet id myTransfersController;

    RECT_ rcKeys92[80];
    
    int mode;
}

- (void)sendKey:(NSPoint)point;

- (void)showKeyboard92:(id)sender;

- (IBAction)clearText:(id)sender;

@end