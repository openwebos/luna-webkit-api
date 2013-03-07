/* @@@LICENSE
*
*      Copyright (c) 2010-2013 Hewlett-Packard Development Company, L.P.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* LICENSE@@@ */




#ifndef IMEDATAINTERFACE_H
#define IMEDATAINTERFACE_H

class VirtualKeyboardPreferences;
class VirtualKeyboard;

#include <lunaservice.h>

#include "IMEData.h"

#if 0

/*

	The IMEData classes are the explicit equivalent to the following template class IMEData<T>
	Unfortunately, qmake doesn't support templates nor macros that define a QObject class.

	Expect the following methods:

	const T & get();				// access the last set value
	void set(const T &);			// change a value, which will trigger the signal...
	void valueChanged(const T &);	// ... to be fired.

*/

template <class T> class IMEData : public QObject
{
	Q_OBJECT

public:
	IMEData() { }

	const T &	get() const				{ return m_value; }
	void		set(const T & value)
	{
		if (!(value == m_value))
		{
			m_value = value;
			Q_EMIT valueChanged(value);
		}
	}

Q_SIGNALS:
	void valueChanged(const T & newValue);

private:
	T	m_value;
};
#endif

/*! \class IMEDataInterface
 *  \brief Interface used by input methods, host and system for communication
 */
class IMEDataInterface : public QObject
{
	Q_OBJECT

public:

	//***************
	//* Host -> IME *
	//***************

    /*! Total screen space. Will change when the device is rotated. Set by host
     *  be used by the input method. */
    IMEData_QSize		m_screenSize;

    /*! Space available for the keyboard in absolute screen coordinates within
     *  the screen space. Set by host to be used by the input method.
     */
    IMEData_QRect		m_availableSpace;

    /*! Tells whether the keyboard should be shown or hidden. Set by host to be
     *  used by the input method.
     */
    IMEData_bool		m_visible;

    /*! Specifies the type of field currently focused. IME should IGNORE the
     *  shiftMode field and only refer to m_shiftMode below! Set by host to be
     *  used by the input method.
     */
    IMEData_EditorState	m_editorState;

    /*! Tells that the SmartKey service will auto-capitalize the next character,
     *  which the keyboard should show. Set by host to be used by the input
     *  method.
     */
    IMEData_bool		m_autoCap;

    /*! \brief Pass a touch event to the input method.
     * \param te Touch event that occurred.
     */
    virtual void touchEvent(const QTouchEvent& te) = 0;

    /*! \brief Notify input method that the screen was tapped.
     * \param tapPt The point of screen that was tapped.
     */
	virtual void tapEvent(const QPoint& tapPt) = 0;

    /*! \brief Make the input method draw itself.
     * \param painter QPainter object to paint with.
     */
    virtual void paint(QPainter& painter) = 0;

    //! \brief Notify input method of a screen edge flick event.
	virtual void screenEdgeFlickEvent() = 0;

	//***************
	//* IME -> Host *
	//***************

    /*! Height of virtual keyboard's main view, which includes the suggestion
     *  picker banner if there's one. Set by the input method to be used by the
     *  host.
     */
    IMEData_qint32		m_keyboardHeight;

    /*! Defines an addition hit region that the IME wants input for. Set by the
     *  input method to be used by the host.
     */
    IMEData_QRegion		m_hitRegion;

    /*! \brief Send a key event from input method to host.
     *  \param type Type of the event.
     *  \param key The key.
     *  \param modifiers Any keyboard modifiers used when the event occurred.
     */
    virtual void sendKeyEvent(QEvent::Type type, Qt::Key key, Qt::KeyboardModifiers modifiers) = 0;

    /*! \brief Trigger the emission of \e signalInvalidateRect(QRect&) -signal.
     *  \param rect The QRect to invalidate.
     */
	virtual void invalidateRect(const QRect& rect) { Q_EMIT signalInvalidateRect(rect); }

    /*! \brief Provide a new word
     * \param text The text to commit, which is either an empty string or a word followed by a space
     */
	virtual void setComposingText(const std::string& text) = 0;

    /*! \todo
     */
    virtual void commitComposingText() = 0;

    /*! \brief Provide a replacement word for the short cut
     * \param text The replacement text
     */
    virtual void commitText(const std::string& text) = 0;

    /*! \brief Request host to perform an editor action, which currently is to move to either the previous or next field
     *  \param action The action to perform, currently either FieldAction_Next or FieldAction_Previous
     */
    virtual void performEditorAction(PalmIME::FieldAction action) = 0;

	//*****************
	//* IME -> System *
	//*****************

    /*! \brief Request the system to hide the input method.
     */
	virtual void requestHide() = 0;

    /*! \brief Check from system if UI animation is active.
     */
    virtual bool isUIAnimationActive() = 0;

    /*! \brief Request system to play audio feedback when key is pressed down.
     *  \param key The key that is pressed down.
     */
    virtual void keyDownAudioFeedback(Qt::Key key) = 0;

    /*! \brief Apply initial settings for the virtual keyboard.
     *  This method must be called by the class that is inherited from
     *  VirtualKeyboard in it's constructor.
     */
    virtual void applyInitSettings(VirtualKeyboard *ime) = 0;

    /*! \brief Activate the current virtual keyboard layout & language combo.
     */
    virtual void activateCombo() = 0;

    /*! \brief Activate the keyboard combo at \a index in the list.
     */
    virtual void selectKeyboardCombo(int index) = 0;

    /*! \brief Activate a keyboard based on it's \a layoutName.
     */
    virtual void selectLayoutCombo(const char * layoutName) = 0;

    /*! \brief Activate the keyboard next on the list.
     */
    virtual void selectNextKeyboardCombo() = 0;

    /*! \brief Creates default keyboards. <em>Use only for testing purposes.</em>
     */
    virtual void createDefaultKeyboards() = 0;

    /*! \brief Clears the default keyboards. <em>Use only for testing purposes.</em>
     */
    virtual void clearDefaultDeyboards() = 0;

    /*! \brief Toggle keyboard tap sounds on or off.
     */
    virtual void toggleTapSounds() = 0;

    /*! \brief Check if tap sounds are on or off.
     */
    virtual bool getTapSounds() const = 0;

    /*! \brief Get the number of keyboard layout & language combos.
     */
    virtual int getKeyboardComboCount() const = 0;

    /*! \brief True if two-spaces are converted to a period.
     */
    virtual bool getSpaces2period() const = 0;

    /*! \brief Select the keyboard's size.
     *  0 is default, 1 is large, -1 is small and -2 is extra small.
     */
    virtual void selectKeyboardSize(int size) = 0;

    /*! \brief Get the language name of the keyboard combo at \a index in the list.
     */
    virtual const char* getLanguageFromKeyboardCombo(int index) = 0;

    /*! \brief Get the layout name of the keyboard combo at \a index in the list.
     */
    virtual const char* getLayoutFromKeyboardCombo(int index) = 0;

Q_SIGNALS:
    /*! \brief Signal the system that an area on the screen should be updated.
     *  \param rect The rectangle on the screen to update.
     */
    void signalInvalidateRect(const QRect& rect);

public:
	virtual ~IMEDataInterface() {}

    //! \brief Gets value of \a key from system settings (luna.conf)
    virtual QVariant getLunaSystemSetting(const QString &key) = 0;

    //! \brief Get localized version of \a str.
    virtual QString getLocalizedString(const std::string &str) = 0;

    //! \brief Get the current locale.
    virtual std::string getLocale() = 0;

    /*! \brief Remove a message banner.
     *  \param appId ID of the application
     *  \param msgId ID of the message to remove
     */
    virtual void createRemoveBannerMessage(const std::string &appId,
                                           const std::string &msgId) = 0;

    /*! \brief Create a message banner.
     *  \param appId ID of the application
     *  \param msg The message
     *  \param launchParams Launch parameters
     *  \param icon Path to icon
     *  \param soundClass Class of the sound to play
     *  \param soundFile Path to sound file to play
     *  \param duration Duration of the sound
     *  \param doNotSuppress True if the message should not be suppressed
     */
    virtual std::string createAddBannerMessage(const std::string &appId,
                                               const std::string &msg,
                                               const std::string &launchParams,
                                               const std::string &icon,
                                               const std::string &soundClass,
                                               const std::string &soundFile,
                                               int duration,
                                               bool doNotSuppress) = 0;

    //! \brief Get virtual keyboard preferences
    virtual VirtualKeyboardPreferences &virtualKeyboardPreferences() = 0;

    //! \brief get luna-service handle
    virtual LSHandle *getLunaServiceHandle() = 0;
};

#endif // IMEDATAINTERFACE_H
