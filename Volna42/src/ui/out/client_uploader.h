const char webdata_client_uploader_html[] PROGMEM = "</script>  <style> #img-up-screen { border: 4px dashed #808080a1; border-radius: 4px; background: #585858; }  .conversion-settings input[type=text] { width : 32px; }  .portrait-mode { transform: rotate(-90deg); margin-top: 50px; }   .landscape-mode-flip { transform: rotate(180deg); margin-top: 50px; }  .portrait-mode-flip { transform: rotate(90deg); margin-top: 50px; }  #disp-info { padding: 10px 0; }  .list-half div:first-child { padding-right: 5px; }  .list-half div { width: 50%; float: left; }  #ui-files-list { margin-bottom: 6px; position : relative; }  #ui-files-list li { margin-bottom: 7px; line-height: 28px; height: 32px; white-space: nowrap; overflow: hidden; }   #disp-info button { display: inline-block; width: auto; margin-right: 6px; }  #ui-files-list li.dselect-item button { display: inline-block; height: 24px; width: auto; float: right; margin-left: 5px; }</style><div id=\"img-up\" style=\"display : none;\"> <button class=\"k-options-additions-show\" data-for=\"disp-main-options\" data-loc=\"cui_general_options\">▼ Основные настройки и картинка</button> <div class=\"k-options-additions-wrap\" id=\"disp-main-options\">  <div class=\"k-options-additions\"> <div class=\"file\"><span data-loc=\"cui_image\">Картинка:</span> <input type=\"file\" id=\"img-up-image\"> </div>  <div id=\"disp-info\"><button id=\"disp-orient\"></button><button id=\"disp-flip\"></button><button id=\"disp-bits\"></button></div>  <div> <label> <select id=\"img-up-fit-mode\"> <option value=\"fitImage\" data-loc=\"cui_resize_fitImage\">Подогнать картинку под размеры экрана</option> <option value=\"fitCover\" data-loc=\"cui_resize_fitCover\">По большей стороне (Cover)</option> <option value=\"width\" data-loc=\"cui_resize_width\">По ширине</option> <option value=\"height\" data-loc=\"cui_resize_height\">По высоте</option> <option value=\"no\" data-loc=\"cui_resize_no\">Вывести без изменений</option> </select> </label> </div>  <div class=\"conversion-settings\">  <div data-loc=\"cui_convert_method\">Метод преобразования</div> <div id=\"bit-1-bit\" > <p> <label> <input type=\"radio\" id=\"img-up-mthreshhold\" name=\"img-up-method\" value=\"threshhold\"> 								<span data-loc=\"cui_convert_mthreshhold\">Пороговое значение</span> <input type=\"text\" id=\"img-up-threshhold-value\" value=\"0.5\" data-step=\"0.01\">  </label> </p> <p> <label> <input type=\"checkbox\" id=\"img-up-invert-color\"><span data-loc=\"cui_convert_invert_color\">Инвертировать цвет картинки</span> </label> </p> <p> <label> <input type=\"checkbox\" id=\"img-up-matkinson-auto\"><span data-loc=\"cui_convert_matkinson_auto\">Ненастраиваемый дизеринг</span>  </label> </p> </div> <div id=\"bit-2-bit\" > <p> <label> <input type=\"radio\" id=\"img-up-mbestmatch\" name=\"img-up-method\" value=\"bestmatch\"> По ближайшим соответствиям </label> </p>  </div> <p> <label> <input type=\"radio\" id=\"img-up-matkinson\" name=\"img-up-method\" value=\"atkinson\" checked> Алгоритм Аткинсона (дизеринг)  <div>Интенсивность <input type=\"range\" id=\"img-up-matkinson-coefficient\" min=\"0\" max=\"1\" step=\"0.1\" value=\"0.125\"></div> <div><label><input type=\"checkbox\" id=\"img-up-gs\"> Доп. преобразования в оттенки серого</label></div> <div><label><input type=\"checkbox\" id=\"img-up-matkinson-drawp\"> Учитывать доп. цвета</label></div> </label> </p> <p> <label><input type=\"checkbox\" id=\"img-up-invert-bg-color\">Инвертировать цвет фона</label> </p>  <p> <label>Смещение по X <input type=\"text\" id=\"img-up-offset-x\" value=\"0\" data-step=\"1\"> Y <input type=\"text\" id=\"img-up-offset-y\" value=\"0\" data-step=\"1\"></label> </p>  </div> </div> </div>  <button class=\"k-options-additions-show\" data-for=\"disp-widget-options\" data-loc=\"cui_widget_options\">▼ Настройки виджетов</button> <div class=\"k-options-additions-wrap\" id=\"disp-widget-options\">  <div class=\"k-options-additions\">  <div class=\"conversion-settings\" id=\"widgets\"></div>  </div>  </div> <button class=\"k-options-additions-show\" data-for=\"disp-ui-options\" id=\"ui-files-manager-opener\" data-loc=\"cui_saved_cui\">▼ Сохраненное</button> <div class=\"k-options-additions-wrap\" id=\"disp-ui-options\">  <div class=\"ui-files-manager\">  <div id=\"ui-files-list\">Список файлов не загружен</div> <div id=\"ui-files-list-info\"></div>			<div><button id=\"ui-files-format\"><span>Очистить память</span></button></div> <!--div class=\"list-half\"><div><button id=\"ui-files-load-list\"><span>Загрузить список файлов</span></button></div><div><button id=\"ui-files-format\"><span>Очистить память</span></button></div></div--> <!--div style=\"clear : both;\"></div-->  <button id=\"ui-files-save\" class=\"k-important\">Сохранить оформление</button> </div>  </div> 	<button class=\"k-options-additions-show\" data-for=\"disp-cui-download\" data-loc=\"cui_download_preview\">▼ Скачать предпросмотр</button> <div class=\"k-options-additions-wrap\" id=\"disp-cui-download\"> <div> <button id=\"img-up-save\"><span>Картинку (BIN)</span></button>			<button id=\"img-up-save-all\"><span>Настройки и картинку (JSON)</span></button> </div>  </div>  <button class=\"k-options-additions-show\" data-for=\"disp-commit-options\" data-loc=\"cui_send_to_screen_options\">▼ Отправка на экран</button> <div class=\"k-options-additions-wrap\" id=\"disp-commit-options\"> <div> <button id=\"img-up-upload\" class=\"k-important\"><span data-loc=\"cui_send_to_screen\">Вывести на экран</span></button>			<button id=\"img-up-reset\"><span>Сбросить экран</span></button> </div>  </div>  <button id=\"img-up-show-preview\" class=\"k-important\" data-loc=\"cui_preview\">Предпросмотр</button></div><div id=\"img-up-preview\" style=\"display : none;\"> <canvas id=\"img-up-screen\"></canvas></div><script>";
const unsigned int webdataSize_client_uploader_html PROGMEM = 6091;