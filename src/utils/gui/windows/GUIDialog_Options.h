/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GUIDialog_Options.h
/// @author  Jakob Erdmann
/// @date    July 2011
///
// A Dialog for setting options (see OptionsCont)
/****************************************************************************/
#ifndef GUIDialog_Options_h
#define GUIDialog_Options_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDialog_Options
 * @briefA Dialog for setting options (see OptionsCont)
 */
class GUIDialog_Options : public FXDialogBox {
public:
    /**@brief Constructor
     *
     * @param[in] parent The parent window
     * @param[in] name The title to show
     * @param[in] width The initial width of the dialog
     * @param[in] height The initial height of the dialog
     */
    GUIDialog_Options(FXWindow* parent,  const char* titleName, int width, int height);

    /// @brief Destructor
    ~GUIDialog_Options();

    // ===========================================================================
    // Option input classes
    // ===========================================================================
    class InputString : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputString)

    public:
        /// @brief constructor
        InputString(FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        FOX_CONSTRUCTOR(InputString)

    private:
        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

    class InputBool : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputBool)

    public:
        /// @brief constructor
        InputBool(FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        FOX_CONSTRUCTOR(InputBool)

    private:
        /// @brief name
        std::string myName;
        /// @brief menu check
        FXMenuCheck* myCheck;
    };


    class InputInt : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputInt)

    public:
        /// @brief
        InputInt(FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        FOX_CONSTRUCTOR(InputInt)

    private:
        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };

    class InputFloat : public FXHorizontalFrame {
        /// @brief FOX-declaration
        FXDECLARE(GUIDialog_Options::InputFloat)

    public:
        /// @brief constructor
        InputFloat(FXComposite* parent, const std::string& name);

        /// @brief try to set new attribute value
        long onCmdSetOption(FXObject*, FXSelector, void*);

    protected:
        FOX_CONSTRUCTOR(InputFloat)

    private:
        /// @brief name
        std::string myName;

        /// @brief text field
        FXTextField* myTextField;
    };
};


#endif

/****************************************************************************/

