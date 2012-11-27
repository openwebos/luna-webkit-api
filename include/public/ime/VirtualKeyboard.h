/* @@@LICENSE
*
*      Copyright (c) 2011-2012 Hewlett-Packard Development Company, L.P.
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

#ifndef VIRTUALKEYBOARD_H
#define VIRTUALKEYBOARD_H

#include <QtPlugin>

#include "IMEDataInterface.h"
#include "InputMethod.h"

/*! \class VirtualKeyboard
 *  \brief Interface for virtual keyboards.
 *
 *  \section vkb_create Creating a new virtual keyboard
 *
 *  \subsection vkb_create_big_picture The big picture:
 *
 *  \li Derive your virtual keyboard class from VirtualKeyboard. VirtualKeyboard
 *      inherits from InputMethod, so you'll need to implement the interfaces of
 *      both classes.
 *
 *  \li Create a factory for it: Derive from VirtualKeyboardFactory and
 *      implement the pure virtual methods.
 *
 *  \li Use IMEDataInterface to communicate with the host system.
 * 
 *  \li The keyboard-efigs repository has an example virtual keyboard plugin.
 *
 *  \subsection vkb_create_notice Smaller things worthy of notice:
 *
 *  \li Apply initial settings for the virtual keyboard at the end of the
 *      constructor:
 *  \code
 *     m_IMEDataInterface->virtualKeyboardPreferences().applyInitSettings(this);
 *  \endcode
 *
 *  \li The keyboard's height needs to be set for the keyboard to show up:
 *  \code
 *     int myKeyboardHeight = 123;
 *     m_IMEDataInterface->m_keyboardHeight.set(myKeyboardHeight);
 *  \endcode
 *  Remember to update this if the keyboard changes it's height.
 *
 */
class VirtualKeyboard : public InputMethod
{
public:
    //! \brief Class constructor.
    explicit VirtualKeyboard(IMEDataInterface *dataInterface) :
        m_IMEDataInterface(dataInterface) {}

    //! \brief Class destructor.
    virtual ~VirtualKeyboard() { m_IMEDataInterface = 0; }

    //! \brief Hide the keyboard.
    void hide() { m_IMEDataInterface->requestHide(); }

    /*! \brief Change the size of the keyboard. Not persistent.
     *  \param size -2 is XS, -1 is S, 0 is M, and 1 is L.
     */
    virtual void requestSize(int size) = 0;

    /*! \brief Change the height of the keyboard. Temporary, not persistent.
     *  \param height Height in pixels.
     */
    virtual void requestHeight(int height) = 0;

    /*! \brief Change the height associated with a size.
     *  Not persistent, but will stick through resizes until the setting is
     *  changed or sysmgr restarted.
     *  \param size -2 is XS, -1 is S, 0 is M, and 1 is L.
     *  \param height Height in pixels.
     */
    virtual void changePresetHeightForSize(int size, int height) = 0;

    /*! For debug purposes, some generic requests can be sent via luna-send
     * commands and processed here
     */
    virtual bool setBoolOption(const std::string &optionName, bool value) = 0;

    /*! For debug purposes, some generic requests can be sent via luna-send
     * commands and processed here
     */
    virtual bool setIntOption(const std::string &optionName, int value) = 0;

    /*! For debug purposes, a named value can be read from the current keyboard.
     */
    virtual bool getValue(const std::string &name, std::string &outValue) = 0;

    /*! \brief Set keyboard layout and language.
     *  \param layoutName Name of the keyboard layout to display
     *  \param languageName Name of language to associate with the layout
     *  \param showLanguageKey Set to true to display the language key in the
     *         layout
     */
    virtual void setKeyboardCombo(const std::string &layoutName,
                                  const std::string &languageName,
                                  bool showLanguageKey) = 0;

    //! \brief Notification that language settings were changed (by the user?)
    virtual void keyboardCombosChanged() = 0;

    //! \brief Get a list of keyboard layouts to be shown in the preference app.
    virtual QList<const char *> getLayoutNameList() = 0;

    /*! \brief Get default language for keyboard layout.
     *  Each layout has a default language.
     *  \param layoutName Name of the layout, should be present in the layout
               name list returned by \e getLayoutNameList().
     *  \return Language string or NULL if the layout is unknown.
     */
    virtual const char *getLayoutDefaultLanguage(const char *layoutName) = 0;

protected:
    //! Pointer to an IMEDataInterface instance.
    IMEDataInterface *m_IMEDataInterface;
};

/*! \class VirtualKeyboardFactory
 * \brief Factory interface for constructing a virtual keyboard.
 */
class VirtualKeyboardFactory
{
public:
    //! Describes how well a virtual keyboard fits to a device.
    typedef enum {
        //! Don't even try.
        eVirtualKeyboardSupport_NotSupported,

        //! Can work, but really not designed for this device.
        eVirtualKeyboardSupport_Poor,

        //! Supports devices of this size.
        eVirtualKeyboardSupport_Preferred_Size,

        //! Supports current locale but fits badly to devices of this size
        eVirtualKeyboardSupport_Preferred_Locale,

        /*! Supports devices of this size and supports current locale
         * particularly well. */
        eVirtualKeyboardSupport_Preferred_SizeAndLocale,

        /*! Supports devices of this size, supports current locale
         * particularly well, and matches the display DPI. */
        eVirtualKeyboardSupport_Preferred_SizeDpiAndLocale

    } EVirtualKeyboardSupport;

    virtual ~VirtualKeyboardFactory() {}

    /*! \brief Get a virtual keyboard.
     * \param dataInterface Pointer to an IMEDataInterface instance.
     * \return A pointer to the virtual keyboard.
     */
    virtual InputMethod *newVirtualKeyboard(IMEDataInterface *dataInterface) = 0;

    /*! \brief Get the name of the keyboard.
     * \return The name of the keyboard.
     */
    virtual QString name() const = 0;

    /*! \brief Get information on how well a virtual keyboard fits to a screen.
     * \param maxWidth Maximum width available for the virtual keyboard in pixels.
     * \param maxHeight Maximum height available for the virtual keyboard in pixels.
     * \param dpi Screen DPI.
     * \param locale Device locale
     */
    virtual EVirtualKeyboardSupport getSupport(int maxWidth,
                                               int maxHeight,
                                               int dpi,
                                               const std::string locale) = 0;
};

Q_DECLARE_INTERFACE(VirtualKeyboardFactory,
                    "com.palm.VirtualKeyboardFactory/1.0")

#define VKB_ENABLE_GLYPH_CACHE 1

// Should always be checked in with 0 for all values...
#define VKB_SHOW_GLYPH_CACHE 0
#define VKB_SHOW_GLYPH_REGIONS 0
#define VKB_FORCE_FPS 0

#endif // VIRTUALKEYBOARD_H
