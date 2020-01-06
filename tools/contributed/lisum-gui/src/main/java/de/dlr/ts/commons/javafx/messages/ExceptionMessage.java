/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2016-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    ExceptionMessage.java
/// @author  Maximiliano Bottazzi
/// @date    2016
///
//
/****************************************************************************/
package de.dlr.ts.commons.javafx.messages;

import java.io.PrintWriter;
import java.io.StringWriter;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.Label;
import javafx.scene.control.TextArea;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Priority;

/**
 *
 * @author @author <a href="mailto:maximiliano.bottazzi@dlr.de">Maximiliano Bottazzi</a>
 */
public class ExceptionMessage extends Message {

    /**
     *
     * @param module
     * @param message
     * @param ex
     */
    public ExceptionMessage(Object module, String message, Exception ex) {
        alert = new Alert(AlertType.ERROR);
        //alert.setTitle("Exception in " + programName);
        alert.setHeaderText(message);
        alert.setContentText("Exception in " + module(module));
        init();

        // Create expandable Exception.
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        ex.printStackTrace(pw);
        String exceptionText = sw.toString();

        Label label = new Label("The exception stacktrace was:");

        TextArea textArea = new TextArea(exceptionText);
        textArea.setEditable(false);
        textArea.setWrapText(true);

        textArea.setMaxWidth(Double.MAX_VALUE);
        textArea.setMaxHeight(Double.MAX_VALUE);
        GridPane.setVgrow(textArea, Priority.ALWAYS);
        GridPane.setHgrow(textArea, Priority.ALWAYS);

        GridPane expContent = new GridPane();
        expContent.setMaxWidth(Double.MAX_VALUE);
        expContent.add(label, 0, 0);
        expContent.add(textArea, 0, 1);

        // Set expandable Exception into the dialog pane.
        alert.getDialogPane().setExpandableContent(expContent);
    }

    /**
     *
     * @param module
     * @return
     */
    private static String module(Object module) {
        String text = null;
        if (module != null) {
            if (module instanceof String) {
                text = (String) module;
            } else {
                text = module.getClass().getSimpleName();
            }
        }

        return text;
    }


}
