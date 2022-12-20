<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.1">
  <compound kind="file">
    <name>gin_adsr.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__adsr_8h.html</filename>
    <class kind="class">ADSR</class>
  </compound>
  <compound kind="file">
    <name>gin_adsrcomponent.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__adsrcomponent_8h.html</filename>
    <class kind="class">ADSRComponent</class>
  </compound>
  <compound kind="file">
    <name>gin_analogadsr.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__analogadsr_8h.html</filename>
    <class kind="class">AnalogADSR</class>
  </compound>
  <compound kind="file">
    <name>gin_asyncutilities.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/utilities/</path>
    <filename>gin__asyncutilities_8h.html</filename>
    <class kind="class">LambdaTimer</class>
    <class kind="class">LambdaAsyncUpdater</class>
    <member kind="function">
      <type>void</type>
      <name>callOnMainThreadBlocking</name>
      <anchorfile>group__gin__gui-utilities.html</anchorfile>
      <anchor>ga9fefd2dceb6369dbeac0b093b14c72fa</anchor>
      <arglist>(std::function&lt; void()&gt; func)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_asyncwebsocket.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_network/network/</path>
    <filename>gin__asyncwebsocket_8h.html</filename>
    <class kind="class">AsyncWebsocket</class>
  </compound>
  <compound kind="file">
    <name>gin_audioequationparser.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__audioequationparser_8h.html</filename>
    <class kind="class">AudioEquationParser</class>
  </compound>
  <compound kind="file">
    <name>gin_audiofifo.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__audiofifo_8h.html</filename>
    <class kind="class">AudioFifo</class>
  </compound>
  <compound kind="file">
    <name>gin_audiofunctions.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__audiofunctions_8h.html</filename>
    <class kind="struct">FuncState</class>
    <class kind="struct">OscState</class>
    <class kind="struct">HP12State</class>
    <class kind="struct">HP24State</class>
    <class kind="struct">BP12State</class>
    <class kind="struct">BP24State</class>
    <class kind="struct">LP12State</class>
    <class kind="struct">LP24State</class>
    <class kind="struct">Notch12State</class>
    <class kind="struct">Notch24State</class>
    <class kind="class">AudioFunctionHost</class>
  </compound>
  <compound kind="file">
    <name>gin_audioutil.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__audioutil_8h.html</filename>
    <class kind="class">GainProcessor</class>
    <class kind="class">WetDryMix</class>
    <member kind="function">
      <type>juce::Value</type>
      <name>findValue</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga642bc58b032577b139e71665cc1817dc</anchor>
      <arglist>(juce::ValueTree &amp;state, juce::Identifier name, juce::var value)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getMidiNoteInHertz</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga0521f87a41bf33dc06cc4b6889860401</anchor>
      <arglist>(const T noteNumber, const T frequencyOfA=(T) 440.0)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getMidiNoteFromHertz</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gaba92011bb25f7416e8752e34fbec9c6f</anchor>
      <arglist>(const T freq, const T frequencyOfA=(T) 440.0)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>velocityToGain</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gaeda2ea585490695813285661264a92b9</anchor>
      <arglist>(float velocity, float sensitivity=1.0f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGain</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gaea524d06c0f9a97e215fafc7e997cc0b</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, juce::LinearSmoothedValue&lt; float &gt; &amp;gain)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGain</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga23129d8a0f12afa042d9788afa21b90f</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, int channel, juce::LinearSmoothedValue&lt; float &gt; &amp;gain)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clip</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga80d41603f74938619f0af6e8007f5944</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, float low=-1.0f, float high=1.0f)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getMidiMessageType</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga6e16d794b5bc34b561ea9107adbd69ce</anchor>
      <arglist>(const juce::MidiMessage &amp;msg)</arglist>
    </member>
    <member kind="function">
      <type>juce::AudioSampleBuffer</type>
      <name>sliceBuffer</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga1f707ada89f48bb4f68c4890ba90c27b</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;input, int start, int length)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>lerp</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga48b0dea68bc2af522095b90b5e9acc13</anchor>
      <arglist>(T a, T b, T t)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>sign</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga1764c1268bc41fe5590a1287c071b27e</anchor>
      <arglist>(T x)</arglist>
    </member>
    <member kind="variable">
      <type>constexpr float</type>
      <name>Q</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gab7b12a212b5068cd4b83c5ef0af70511</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_bandlimitedlookuptable.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__bandlimitedlookuptable_8h.html</filename>
    <class kind="class">BandLimitedLookupTable</class>
    <class kind="class">BandLimitedLookupTables</class>
    <member kind="enumeration">
      <type></type>
      <name>Wave</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gadb931b2f752b7879df254a7fd9d17e90</anchor>
      <arglist></arglist>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a0e82533c37fced763be34a5c21d720c9">silence</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a925212d1624625aed9491413b4b7be05">sine</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90acef44b46f16ae8ecf664df4266ffdbf9">triangle</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90acf87f9eb2bfacc5a0362031eba539290">sawUp</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a3593df94728cd88f078bbc6c41df9876">sawDown</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a02a8dc4cf01fed584c6423f577c0b0d7">pulse</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a2fc01ec765ec0cb3dcc559126de20b30">square</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90aaaddc3454ccbefbb2d8d8461f8f7f481">noise</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a6f0316b89c2034b8fabe6d3e48f15bbf">wavetable</enumvalue>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sine</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gac81d5fa67210b71931d9191c641871b2</anchor>
      <arglist>(double phase, double unused1=0, double unused2=0)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>triangle</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga83fe6fa8512b1a676092cf10f9c79e7e</anchor>
      <arglist>(double phase, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sawUp</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga54fd9fc5216539143983df52fcfd38be</anchor>
      <arglist>(double phase, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sawDown</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gaf4e429b20b8c3d03b8527af2f1d7a14f</anchor>
      <arglist>(double phase, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>pulse</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga5d70ff16cbda6f79543b487e6831052a</anchor>
      <arglist>(double phase, double pw, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>squareWave</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gad6edce63379d418ac8ceb7f57a97bfc6</anchor>
      <arglist>(double phase, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>noise</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga144d0a5afb304f562a311e0d62e77dc6</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_bmpimageformat.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_graphics/formats/</path>
    <filename>gin__bmpimageformat_8h.html</filename>
    <class kind="class">BMPImageFormat</class>
  </compound>
  <compound kind="file">
    <name>gin_coalescedtimer.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/utilities/</path>
    <filename>gin__coalescedtimer_8h.html</filename>
    <class kind="class">CoalescedTimer</class>
  </compound>
  <compound kind="file">
    <name>gin_commentmetadata.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_metadata/metadata/</path>
    <filename>gin__commentmetadata_8h.html</filename>
    <class kind="class">CommentMetadata</class>
  </compound>
  <compound kind="file">
    <name>gin_componentutils.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/components/</path>
    <filename>gin__componentutils_8h.html</filename>
    <member kind="function">
      <type>void</type>
      <name>addAndMakeVisible</name>
      <anchorfile>group__gin__gui-components.html</anchorfile>
      <anchor>ga66410efb088d24a02a86b4c6191e2866</anchor>
      <arglist>(juce::Component &amp;parent, juce::Array&lt; juce::Component * &gt; children)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addAndMakeVisible</name>
      <anchorfile>group__gin__gui-components.html</anchorfile>
      <anchor>gaa5dca7c16023c4ec901f0db44aaf89de</anchor>
      <arglist>(juce::Component &amp;parent, std::initializer_list&lt; juce::Component * &gt; children)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_componentviewer.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/components/</path>
    <filename>gin__componentviewer_8h.html</filename>
    <class kind="class">ComponentViewer</class>
  </compound>
  <compound kind="file">
    <name>gin_controlbox.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__controlbox_8h.html</filename>
    <class kind="class">ControlBox</class>
    <class kind="class">PagedControlBox</class>
  </compound>
  <compound kind="file">
    <name>gin_copperlookandfeel.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/lookandfeel/</path>
    <filename>gin__copperlookandfeel_8h.html</filename>
    <includes id="gin__images_8h" name="gin_images.h" local="yes" imported="no">gin_images.h</includes>
    <class kind="class">CopperLookAndFeel</class>
    <class kind="class">CopperLookAndFeelWrapper</class>
  </compound>
  <compound kind="file">
    <name>gin_delayline.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__delayline_8h.html</filename>
    <class kind="class">DelayLine</class>
  </compound>
  <compound kind="file">
    <name>gin_distortion.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__distortion_8h.html</filename>
    <class kind="class">Distortion</class>
  </compound>
  <compound kind="file">
    <name>gin_downloadmanager.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__downloadmanager_8h.html</filename>
    <includes id="utilities_2gin__util_8h" name="utilities/gin_util.h" local="yes" imported="no">gin_util.h</includes>
    <class kind="class">DownloadManager</class>
    <class kind="struct">DownloadManager::DownloadResult</class>
  </compound>
  <compound kind="file">
    <name>gin_dynamics.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__dynamics_8h.html</filename>
    <class kind="class">EnvelopeDetector</class>
    <class kind="class">Dynamics</class>
  </compound>
  <compound kind="file">
    <name>gin_dynamicsmeter.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/components/</path>
    <filename>gin__dynamicsmeter_8h.html</filename>
    <class kind="class">DynamicsMeter</class>
  </compound>
  <compound kind="file">
    <name>gin_easedvaluesmoother.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__easedvaluesmoother_8h.html</filename>
    <class kind="class">EasedValueSmoother</class>
  </compound>
  <compound kind="file">
    <name>gin_easing.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__easing_8h.html</filename>
    <member kind="function">
      <type>T</type>
      <name>easeLinear</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga7646e77db052bff3178b73c722206e52</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuadraticIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga033a11422391bbad3d495e863729565d</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuadraticOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga70b838d26aa2fa360932a8716b6ff18f</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuadraticInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gabfebd9814de3e919edb3cd7516b9050d</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCubicIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gadd816be517ee34d5d5d6c03f58e6d615</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCubicOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga0a5b2a4c5ec14e5dd08e5c97ae5c7593</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCubicInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaaee23821c48a7585e68041e964fd8b2e</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuarticIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gae7d3fcf4936075991a42ec05b1ab1c53</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuarticOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga0141f33c3cea06bd2b792fd1aec21987</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuarticInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga8a97392f7cf1947e253594c713c73969</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuinticIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga25b4d43e01068724a839dc92f333f24c</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuinticOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga499817d290b5d1d7c0b83c5e0c2f80ac</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuinticInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gac2867f6fb639343e139634bf45c4d106</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeSineIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga5ff43623fe37daf28fc4dea493beb3a4</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeSineOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gab7eda190492ca09c29cc0777ac6451cf</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeSineInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga12bf8c86b1b6627bf353ff742d7d711e</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCircularIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga101b272b8b327afdd7b73be2da6d0c6b</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCircularOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga1b69a1d31e0fdc30a209976965266970</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCircularInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga59090fad80e0b20c15074345995c21dd</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeExponentialIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaf1d0fd36995e8a9b9401a5966afeccb3</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeExponentialOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gac84889aa5e141b6fb2185e1877be552a</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeExponentialInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga66172ec142beaa1682ec8800a2e968bf</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeElasticIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaa583a283fcdb33f6658e7d5f45f9ea86</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeElasticOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga5d166785781b41c81e743e689b7b3314</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeElasticInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gabb5639d85f795ff77e4b553ab9dfcf1c</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBackIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga2fe38f966343690da01444f64772cd7a</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBackOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaa8cce7b392a0034c4c947b07a94dab45</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBackInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga0546a40d7cf9b955aa1ea464ea2e9129</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBounceIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga053d2907e791ee49187e6dfee114952b</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBounceOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gab198cd18c0de013a626ab39fd09917ce</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBounceInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga16564d349e6fe228db0c8b22df5d7bb4</anchor>
      <arglist>(T p)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_elevatedfilecopy.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/utilities/</path>
    <filename>gin__elevatedfilecopy_8h.html</filename>
    <class kind="class">ElevatedFileCopy</class>
  </compound>
  <compound kind="file">
    <name>gin_eq.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__eq_8h.html</filename>
    <class kind="class">EQ</class>
  </compound>
  <compound kind="file">
    <name>gin_equationparser.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__equationparser_8h.html</filename>
    <class kind="class">EquationParser</class>
  </compound>
  <compound kind="file">
    <name>gin_exifmetadata.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_metadata/metadata/</path>
    <filename>gin__exifmetadata_8h.html</filename>
    <class kind="class">ExifMetadata</class>
  </compound>
  <compound kind="file">
    <name>gin_filesystemwatcher.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__filesystemwatcher_8h.html</filename>
    <class kind="class">FileSystemWatcher</class>
    <class kind="class">FileSystemWatcher::Listener</class>
  </compound>
  <compound kind="file">
    <name>gin_fileutilities.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__fileutilities_8h.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>overwriteWithText</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga1aac7c94939802d4c2c08d600feefa23</anchor>
      <arglist>(const juce::File &amp;f, const juce::String &amp;text, bool asUnicode=false, bool writeUnicodeHeaderBytes=false, const char *lineEndings=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>overwriteWithData</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga6e2dbf5f19342f51ea56d96cb5e9e842</anchor>
      <arglist>(const juce::File &amp;f, const juce::MemoryBlock &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>overwriteWithData</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gae1a4b4ab413ade0e4c90ad3d26ce854b</anchor>
      <arglist>(const juce::File &amp;f, const void *data, size_t size)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_filter.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__filter_8h.html</filename>
    <class kind="class">Filter</class>
  </compound>
  <compound kind="file">
    <name>gin_gateeffect.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__gateeffect_8h.html</filename>
    <class kind="class">GateEffect</class>
  </compound>
  <compound kind="file">
    <name>gin_gateeffectcomponent.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__gateeffectcomponent_8h.html</filename>
    <class kind="class">GateEffectComponent</class>
  </compound>
  <compound kind="file">
    <name>gin_geometry.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/geometry/</path>
    <filename>gin__geometry_8h.html</filename>
    <class kind="class">Ellipse</class>
    <member kind="function">
      <type>T</type>
      <name>square</name>
      <anchorfile>group__gin-geometry.html</anchorfile>
      <anchor>ga1c4cf92c230b2e375db8d832d07ec041</anchor>
      <arglist>(T in)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>solveLine</name>
      <anchorfile>group__gin-geometry.html</anchorfile>
      <anchor>ga97bafa72f9f8ecdba9b839ecdb3db767</anchor>
      <arglist>(T x1, T y1, T x2, T y2, T &amp;m, T &amp;b)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_ginlookandfeel.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/components/</path>
    <filename>gin__ginlookandfeel_8h.html</filename>
    <class kind="class">GinLookAndFeel</class>
  </compound>
  <compound kind="file">
    <name>gin_headers.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__headers_8h.html</filename>
    <class kind="class">HeaderItem</class>
    <class kind="class">HeaderRow</class>
  </compound>
  <compound kind="file">
    <name>gin_http.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_network/network/</path>
    <filename>gin__http_8h.html</filename>
    <class kind="class">Http</class>
    <class kind="struct">Http::HttpResult</class>
  </compound>
  <compound kind="file">
    <name>gin_imageeffects.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_graphics/images/</path>
    <filename>gin__imageeffects_8h.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>BlendMode</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga30323fa90e24ca484998b9fba6c40e85</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Normal</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a5ecbcf0afce98b042f35ec71ba03fa4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Lighten</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a0c4863f7d789ab70b21073bd94c9169d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Darken</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a95ea73d8fed63115639319a67d596d49</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Multiply</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ade55cee401a6a53619b6ec469abf0810</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Average</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ab3c87ec2c47256239220b24e46acda7f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Add</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a0d592a4b562059bc283e00b3704865c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Subtract</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85afa8a9720422ee254970b2ebe1435a917</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Difference</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a9cba343e00a6259dc83f7e4e7f447109</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Negation</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ab94b0efce58a77e1c8fa29e32536c728</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Screen</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a2609fadb77bb3d98c615b41ac79a1d31</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Exclusion</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ac200cda01c78d8dae2b11cf4ded47655</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Overlay</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a8b895d92539a718e6898ed6b053ce2ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SoftLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a967749a14f4bb7ffc6383997d97a6163</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HardLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a7fc75582ae37ab60f5a1cc8e2acd7e24</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ColorDodge</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a71b5386d914bd4101f3d086d8ce72161</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ColorBurn</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85adccc18afee7dc5a40147cab59118d583</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LinearDodge</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85afe7aab6a1580381724d2ebe724fbd065</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LinearBurn</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a4c41b912f1e9eca71a1a36fc8a1172d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LinearLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ae158a552ecb4c88077b7537e93a8df86</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>VividLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a87be187db30f3efdb79dde93f63e0430</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>PinLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a513bf9b05af260d001632c4fe6d345d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HardMix</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a76ee1ad58cc676f7994416354a4d7b59</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Reflect</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a8898819fa0ebd77a64e192d47e7be553</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Glow</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ac7adb89e56ad4a2daef183d318110442</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Phoenix</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a85c155bffb4118332192060f1a6db976</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyVignette</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga1023b87ad7bb7ca1e039ded54397ef90</anchor>
      <arglist>(juce::Image &amp;img, float amount, float radius, float falloff, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applySepia</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gaccaa511049769192c15aa13e0c2a40d1</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGreyScale</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gadb285c05d7e81fbffe8b2ab5705b0783</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applySoften</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga4cc9c0919994beeac34d088beb1991e2</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applySharpen</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gac0b5514a6c74e31bd4e7a26898e45228</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGamma</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga2b20d743c3475fd86d18a8f2cb427b47</anchor>
      <arglist>(juce::Image &amp;img, float gamma, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyInvert</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga6453f04ab3d2a6a345cd6b7f70711c04</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyContrast</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga50c77bf67a02fbc002d1136fd64f2031</anchor>
      <arglist>(juce::Image &amp;img, float contrast, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyBrightnessContrast</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gab3825059a9e75bfc8e78cf0106ddf78e</anchor>
      <arglist>(juce::Image &amp;img, float brightness, float contrast, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyHueSaturationLightness</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga25205b0926bfca178115404c6d9f45d9</anchor>
      <arglist>(juce::Image &amp;img, float hue, float saturation, float lightness, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyStackBlur</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga5298c1220caef0f1d6d918c9ec15243d</anchor>
      <arglist>(juce::Image &amp;img, int radius)</arglist>
    </member>
    <member kind="function">
      <type>juce::Image</type>
      <name>applyResize</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga8a045522d5a03954f0d4fe2cbc8082a4</anchor>
      <arglist>(const juce::Image &amp;img, int width, int height)</arglist>
    </member>
    <member kind="function">
      <type>juce::Image</type>
      <name>applyResize</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga99654c39e56b66a36c75cbbff9e137b4</anchor>
      <arglist>(const juce::Image &amp;img, float factor)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGradientMap</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga2e91627e4ee0f9e1aeca78ca387a8c51</anchor>
      <arglist>(juce::Image &amp;img, const juce::ColourGradient &amp;gradient, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGradientMap</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga48f6d1c24d18edd26e2d9da48e47dcf7</anchor>
      <arglist>(juce::Image &amp;img, const juce::Colour c1, const juce::Colour c2, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyColour</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gae6d6ea52612fb4f6474b05a1063604db</anchor>
      <arglist>(juce::Image &amp;img, juce::Colour c, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyBlend</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga6e16e6b71a97615070d0f5f6829f2b04</anchor>
      <arglist>(juce::Image &amp;dst, const juce::Image &amp;src, BlendMode mode, float alpha=1.0f, juce::Point&lt; int &gt; position={}, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyBlend</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga590207f6293f03a5a4e4e2a1d4f3db15</anchor>
      <arglist>(juce::Image &amp;dst, BlendMode mode, juce::Colour c, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_imagemetadata.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_metadata/metadata/</path>
    <filename>gin__imagemetadata_8h.html</filename>
    <class kind="class">ImageMetadata</class>
  </compound>
  <compound kind="file">
    <name>gin_images.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/lookandfeel/</path>
    <filename>gin__images_8h.html</filename>
    <class kind="class">Images</class>
  </compound>
  <compound kind="file">
    <name>gin_imageutilities.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/images/</path>
    <filename>gin__imageutilities_8h.html</filename>
    <member kind="function">
      <type>juce::Image</type>
      <name>rasterizeSVG</name>
      <anchorfile>group__gin__gui-images.html</anchorfile>
      <anchor>gaad1e06e3c606f89456dac559e0d417a7</anchor>
      <arglist>(juce::String svgText, int w, int h)</arglist>
    </member>
    <member kind="function">
      <type>juce::Path</type>
      <name>parseSVGPath</name>
      <anchorfile>group__gin__gui-images.html</anchorfile>
      <anchor>ga781250a0ec9422dc8c7365cf6b9935ee</anchor>
      <arglist>(const juce::String &amp;txt)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_integrator.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__integrator_8h.html</filename>
    <class kind="class">Integrator</class>
  </compound>
  <compound kind="file">
    <name>gin_iptcmetadata.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_metadata/metadata/</path>
    <filename>gin__iptcmetadata_8h.html</filename>
    <class kind="class">IptcMetadata</class>
  </compound>
  <compound kind="file">
    <name>gin_knob.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__knob_8h.html</filename>
    <class kind="class">Knob</class>
  </compound>
  <compound kind="file">
    <name>gin_lagrange.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__lagrange_8h.html</filename>
    <namespace>Lagrange</namespace>
    <member kind="function">
      <type>T</type>
      <name>interpolate</name>
      <anchorfile>namespaceLagrange.html</anchorfile>
      <anchor>aed0cc45b276f9ed337143c4587dc1122</anchor>
      <arglist>(const juce::Array&lt; Point&lt; T &gt;&gt; &amp;points, T x)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>interpolate</name>
      <anchorfile>namespaceLagrange.html</anchorfile>
      <anchor>ab812f4c6079c4c45b43e56239c3634d4</anchor>
      <arglist>(T xArr[], T yArr[], int num, T x)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_layout.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/utilities/</path>
    <filename>gin__layout_8h.html</filename>
    <class kind="class">Layout</class>
  </compound>
  <compound kind="file">
    <name>gin_leastsquaresregression.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__leastsquaresregression_8h.html</filename>
    <class kind="class">LeastSquaresRegression</class>
  </compound>
  <compound kind="file">
    <name>gin_levelmeter.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/components/</path>
    <filename>gin__levelmeter_8h.html</filename>
    <class kind="class">LevelMeter</class>
  </compound>
  <compound kind="file">
    <name>gin_leveltracker.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__leveltracker_8h.html</filename>
    <class kind="class">LevelTracker</class>
  </compound>
  <compound kind="file">
    <name>gin_lfo.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__lfo_8h.html</filename>
    <class kind="class">LFO</class>
    <class kind="struct">LFO::Parameters</class>
  </compound>
  <compound kind="file">
    <name>gin_lfocomponent.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__lfocomponent_8h.html</filename>
    <class kind="class">LFOComponent</class>
  </compound>
  <compound kind="file">
    <name>gin_linearregression.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__linearregression_8h.html</filename>
    <class kind="class">LinearRegression</class>
  </compound>
  <compound kind="file">
    <name>gin_mapviewer.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/components/</path>
    <filename>gin__mapviewer_8h.html</filename>
    <class kind="class">MapViewer</class>
  </compound>
  <compound kind="file">
    <name>gin_messagepack.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__messagepack_8h.html</filename>
    <class kind="class">MessagePack</class>
  </compound>
  <compound kind="file">
    <name>gin_midififo.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__midififo_8h.html</filename>
    <class kind="class">MidiFifo</class>
  </compound>
  <compound kind="file">
    <name>gin_modmatrix.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/plugin/</path>
    <filename>gin__modmatrix_8h.html</filename>
    <class kind="struct">ModSrcId</class>
    <class kind="struct">ModDstId</class>
    <class kind="class">ModVoice</class>
    <class kind="class">ModMatrix</class>
    <class kind="class">ModMatrix::Listener</class>
  </compound>
  <compound kind="file">
    <name>gin_dsp/dsp/gin_modulation.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__dsp_2dsp_2gin__modulation_8h.html</filename>
    <class kind="class">Modulation</class>
  </compound>
  <compound kind="file">
    <name>gin_plugin/components/gin_modulation.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__plugin_2components_2gin__modulation_8h.html</filename>
    <class kind="class">ModulationSourceButton</class>
    <class kind="class">ModulationDestinationButton</class>
    <class kind="class">ModulationOverview</class>
    <class kind="class">ModSrcListBox</class>
    <class kind="class">ModMatrixBox</class>
  </compound>
  <compound kind="file">
    <name>gin_modules.dox</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/</path>
    <filename>gin__modules_8dox.html</filename>
  </compound>
  <compound kind="file">
    <name>gin_multiparamcomponent.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__multiparamcomponent_8h.html</filename>
    <class kind="class">MultiParamComponent</class>
  </compound>
  <compound kind="file">
    <name>gin_noteduration.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/plugin/</path>
    <filename>gin__noteduration_8h.html</filename>
    <class kind="class">NoteDuration</class>
  </compound>
  <compound kind="file">
    <name>gin_oauth.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_network/network/</path>
    <filename>gin__oauth_8h.html</filename>
  </compound>
  <compound kind="file">
    <name>gin_openstreetmaps.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/utilities/</path>
    <filename>gin__openstreetmaps_8h.html</filename>
    <class kind="class">OpenStreetMaps</class>
    <class kind="class">OpenStreetMaps::Listener</class>
  </compound>
  <compound kind="file">
    <name>gin_oscillators.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__oscillators_8h.html</filename>
    <class kind="class">StereoOscillator</class>
    <class kind="struct">StereoOscillator::Params</class>
    <class kind="class">VoicedStereoOscillator</class>
    <class kind="struct">VoicedStereoOscillator::Params</class>
    <class kind="class">BLLTVoicedStereoOscillator</class>
  </compound>
  <compound kind="file">
    <name>gin_parambox.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__parambox_8h.html</filename>
    <class kind="class">ParamHeader</class>
    <class kind="class">ParamBox</class>
    <class kind="class">ParamArea</class>
    <class kind="class">BoxArea</class>
    <member kind="function">
      <type>void</type>
      <name>gradientRect</name>
      <anchorfile>group__gin__plugin-components.html</anchorfile>
      <anchor>gae617c6797d9252a57e49dc7bef2f487b</anchor>
      <arglist>(juce::Graphics &amp;g, juce::Rectangle&lt; int &gt; rc, juce::Colour c1, juce::Colour c2)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_parameter.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/plugin/</path>
    <filename>gin__parameter_8h.html</filename>
    <class kind="class">Parameter</class>
    <class kind="class">Parameter::ParameterListener</class>
    <class kind="struct">Parameter::ParamState</class>
  </compound>
  <compound kind="file">
    <name>gin_patchbrowser.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__patchbrowser_8h.html</filename>
    <class kind="class">PatchBrowser</class>
    <class kind="struct">PatchBrowser::AuthorsModel</class>
    <class kind="struct">PatchBrowser::TagsModel</class>
    <class kind="struct">PatchBrowser::PresetsModel</class>
  </compound>
  <compound kind="file">
    <name>gin_plist.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__plist_8h.html</filename>
    <member kind="function">
      <type>juce::var</type>
      <name>parsePlist</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gac2aec1c96e46e294133bec5727ad2e98</anchor>
      <arglist>(const juce::File &amp;f)</arglist>
    </member>
    <member kind="function">
      <type>juce::var</type>
      <name>parsePlist</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gab0ff62a7c0a17d71004b40eb5a56f6a4</anchor>
      <arglist>(const juce::String &amp;s)</arglist>
    </member>
    <member kind="function">
      <type>juce::var</type>
      <name>parsePlist</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga745860e41e38bb0e8b5e52cd7655628a</anchor>
      <arglist>(const juce::XmlElement &amp;f)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_pluginalertwindow.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__pluginalertwindow_8h.html</filename>
    <class kind="class">PluginAlertWindow</class>
  </compound>
  <compound kind="file">
    <name>gin_plugincomponents.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__plugincomponents_8h.html</filename>
    <class kind="class">PluginButton</class>
    <class kind="class">PowerButton</class>
    <class kind="class">PluginComboBox</class>
    <class kind="class">PluginSlider</class>
    <class kind="class">Readout</class>
    <class kind="class">ParamComponent</class>
    <class kind="class">HorizontalFader</class>
    <class kind="class">Switch</class>
    <class kind="class">Select</class>
    <class kind="class">SynthesiserUsage</class>
  </compound>
  <compound kind="file">
    <name>gin_plugineditor.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__plugineditor_8h.html</filename>
    <class kind="class">UpdateChecker</class>
    <class kind="class">NewsChecker</class>
    <class kind="class">ProcessorEditorBase</class>
    <class kind="class">TitleBar</class>
    <class kind="class">ProcessorEditor</class>
  </compound>
  <compound kind="file">
    <name>gin_pluginlookandfeel.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/lookandfeel/</path>
    <filename>gin__pluginlookandfeel_8h.html</filename>
    <class kind="class">PluginLookAndFeel</class>
    <class kind="class">PluginLookAndFeelWrapper</class>
  </compound>
  <compound kind="file">
    <name>gin_point.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__point_8h.html</filename>
    <class kind="class">Point</class>
  </compound>
  <compound kind="file">
    <name>gin_processor.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/plugin/</path>
    <filename>gin__processor_8h.html</filename>
    <includes id="gin__parameter_8h" name="gin_parameter.h" local="yes" imported="no">gin_parameter.h</includes>
    <includes id="gin__program_8h" name="gin_program.h" local="yes" imported="no">gin_program.h</includes>
    <includes id="gin__pluginlookandfeel_8h" name="gin_pluginlookandfeel.h" local="yes" imported="no">../lookandfeel/gin_pluginlookandfeel.h</includes>
    <includes id="gin__copperlookandfeel_8h" name="gin_copperlookandfeel.h" local="yes" imported="no">../lookandfeel/gin_copperlookandfeel.h</includes>
    <class kind="class">SmoothingType</class>
    <class kind="class">Processor</class>
    <member kind="define">
      <type>#define</type>
      <name>ProcessorBaseClass</name>
      <anchorfile>group__gin__plugin-plugin.html</anchorfile>
      <anchor>ga9ccf6c679c1fdf1c1414992a17202f5a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_program.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/plugin/</path>
    <filename>gin__program_8h.html</filename>
    <includes id="gin__parameter_8h" name="gin_parameter.h" local="yes" imported="no">gin_parameter.h</includes>
    <class kind="class">Program</class>
  </compound>
  <compound kind="file">
    <name>gin_propertycomponents.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/components/</path>
    <filename>gin__propertycomponents_8h.html</filename>
    <class kind="class">PropertyComponentBase</class>
    <class kind="class">FilePropertyComponent</class>
    <class kind="class">ColourPropertyComponent</class>
  </compound>
  <compound kind="file">
    <name>gin_realtimeasyncupdater.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__realtimeasyncupdater_8h.html</filename>
    <class kind="class">RealtimeAsyncUpdater</class>
  </compound>
  <compound kind="file">
    <name>gin_resamplingfifo.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__resamplingfifo_8h.html</filename>
    <includes id="gin__audioutil_8h" name="gin_audioutil.h" local="yes" imported="no">gin_audioutil.h</includes>
    <class kind="class">ResamplingFifo</class>
  </compound>
  <compound kind="file">
    <name>gin_resources.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/lookandfeel/</path>
    <filename>gin__resources_8h.html</filename>
    <namespace>Resources</namespace>
    <member kind="variable">
      <type>const char *</type>
      <name>BarlowRegular_ttf</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>abbddb191fa7624c2d2127ce3b537b898</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>BarlowRegular_ttfSize</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>ab6e0bb1c70ec3ed211118f968880cd2f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>BarlowThin_ttf</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>ae4ad6d277ad33de26e02e1572510d8dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>BarlowThin_ttfSize</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>ae17a5aae2e3106082be4c8b5d3e61bf9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>InterRegular_otf</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>a821eb0a806b852016c53ab6151f00d3d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>InterRegular_otfSize</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>af4f0aa394cb30c14c4306162f0d5e6d4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_sample.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__sample_8h.html</filename>
    <class kind="class">Sample</class>
  </compound>
  <compound kind="file">
    <name>gin_sampleoscillator.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__sampleoscillator_8h.html</filename>
    <class kind="class">SampleOscillator</class>
  </compound>
  <compound kind="file">
    <name>gin_scratchbuffer.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__scratchbuffer_8h.html</filename>
    <class kind="class">ScratchBuffer</class>
  </compound>
  <compound kind="file">
    <name>gin_securestreamingsocket.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_network/network/</path>
    <filename>gin__securestreamingsocket_8h.html</filename>
    <class kind="class">SecureStreamingSocket</class>
  </compound>
  <compound kind="file">
    <name>gin_sharedmemory.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__sharedmemory_8h.html</filename>
    <class kind="class">SharedMemory</class>
  </compound>
  <compound kind="file">
    <name>gin_simpleverb.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__simpleverb_8h.html</filename>
    <class kind="class">SimpleVerb</class>
  </compound>
  <compound kind="file">
    <name>gin_singlelinetexteditor.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/components/</path>
    <filename>gin__singlelinetexteditor_8h.html</filename>
    <class kind="class">SingleLineTextEditor</class>
    <class kind="class">SingleLineTextEditor::Listener</class>
    <class kind="class">SingleLineTextEditor::InputFilter</class>
    <class kind="class">SingleLineTextEditor::LengthAndCharacterRestriction</class>
    <class kind="struct">SingleLineTextEditor::LookAndFeelMethods</class>
  </compound>
  <compound kind="file">
    <name>gin_smoothedparameter.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/plugin/</path>
    <filename>gin__smoothedparameter_8h.html</filename>
    <class kind="class">SmoothedParameter</class>
  </compound>
  <compound kind="file">
    <name>gin_spline.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__spline_8h.html</filename>
    <class kind="class">Spline</class>
    <class kind="class">Spline::Element</class>
  </compound>
  <compound kind="file">
    <name>gin_steplfo.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__steplfo_8h.html</filename>
    <class kind="class">StepLFO</class>
  </compound>
  <compound kind="file">
    <name>gin_steplfocomponent.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_plugin/components/</path>
    <filename>gin__steplfocomponent_8h.html</filename>
    <class kind="class">StepLFOComponent</class>
  </compound>
  <compound kind="file">
    <name>gin_stereodelay.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__stereodelay_8h.html</filename>
    <class kind="class">StereoDelay</class>
  </compound>
  <compound kind="file">
    <name>gin_stereoenhancer.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__stereoenhancer_8h.html</filename>
    <class kind="class">StereoEnhancer</class>
  </compound>
  <compound kind="file">
    <name>gin_stereoprocessor.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__stereoprocessor_8h.html</filename>
    <class kind="class">StereoProcessor</class>
  </compound>
  <compound kind="file">
    <name>gin_synthesiser.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__synthesiser_8h.html</filename>
    <class kind="struct">GlideInfo</class>
    <class kind="class">SynthesiserVoice</class>
    <class kind="class">Synthesiser</class>
  </compound>
  <compound kind="file">
    <name>gin_systemsemaphore.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__systemsemaphore_8h.html</filename>
    <class kind="class">SystemSemaphore</class>
  </compound>
  <compound kind="file">
    <name>gin_textrenderer.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__textrenderer_8h.html</filename>
    <class kind="class">TextRenderer</class>
  </compound>
  <compound kind="file">
    <name>gin_threading.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__threading_8h.html</filename>
    <member kind="function">
      <type>void</type>
      <name>callInBackground</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gafe89b1694e591a29d7c325e41b21543f</anchor>
      <arglist>(std::function&lt; void(void)&gt; function)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>multiThreadedFor</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga383aa0fdd6752ac48ba3e087006730fa</anchor>
      <arglist>(T start, T end, T interval, juce::ThreadPool *threadPool, std::function&lt; void(T idx)&gt; callback)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_triggeredscope.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/components/</path>
    <filename>gin__triggeredscope_8h.html</filename>
    <class kind="class">TriggeredScope</class>
  </compound>
  <compound kind="file">
    <name>utilities/gin_util.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>utilities_2gin__util_8h.html</filename>
    <class kind="class">PerlinNoise</class>
    <class kind="class">RollingAverage</class>
    <class kind="class">TimeProfiler</class>
    <member kind="function">
      <type>bool</type>
      <name>compareAndReset</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga1ebc11c7a7ef780026ec5a519893895e</anchor>
      <arglist>(bool &amp;flag)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>calculateRMS</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga458ba95824635202e48f00c71ced463d</anchor>
      <arglist>(const float *values, int n)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>calculateMedian</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga823d49b3f21b97485eed223caec940c6</anchor>
      <arglist>(const float *values, int n)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>shuffleArray</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaac60b7f594994fac0cb43d317f794449</anchor>
      <arglist>(juce::Random &amp;r, T array)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>almostEqual</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga06700101fa042b926bad6b16738aff53</anchor>
      <arglist>(T a, T b, T precision=T(0.00001))</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>versionStringToInt</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga9861500c17d0717e211dc67c84ccc261</anchor>
      <arglist>(const juce::String &amp;versionString)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>delayedLambda</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaa8e8a032e70b00eb8b977cadd0fb09a8</anchor>
      <arglist>(std::function&lt; void()&gt; callback, int delayMS)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>formatNumber</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaebf910d77d287573a8f98f51c67f8d48</anchor>
      <arglist>(T v)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gui/utilities/gin_util.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_gui/utilities/</path>
    <filename>gui_2utilities_2gin__util_8h.html</filename>
    <class kind="class">AsyncDownload</class>
    <member kind="function">
      <type>juce::Colour</type>
      <name>goldenRatioColor</name>
      <anchorfile>group__gin__gui-utilities.html</anchorfile>
      <anchor>ga345f5b8f3ffdd680b206fe42f25be481</anchor>
      <arglist>(int idx)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_valuesmoother.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__valuesmoother_8h.html</filename>
    <class kind="class">ValueSmoother</class>
  </compound>
  <compound kind="file">
    <name>gin_valuetreeobject.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__valuetreeobject_8h.html</filename>
    <class kind="class">ValueTreeObject</class>
  </compound>
  <compound kind="file">
    <name>gin_valuetreeutilities.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin/utilities/</path>
    <filename>gin__valuetreeutilities_8h.html</filename>
    <class kind="class">LambdaValueTreeListener</class>
    <class kind="class">AsyncLambdaValueTreeListener</class>
    <member kind="function">
      <type>juce::String</type>
      <name>valueTreeToJSON</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gac8809f5a07d326dc76f18d09bd99db9d</anchor>
      <arglist>(const juce::ValueTree &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>juce::ValueTree</type>
      <name>valueTreeFromJSON</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga88f482bf81fb3e423fa1e3926dfaa9a6</anchor>
      <arglist>(const juce::String &amp;jsonText)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_waveformcomponent.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/components/</path>
    <filename>gin__waveformcomponent_8h.html</filename>
    <class kind="class">WaveformComponent</class>
  </compound>
  <compound kind="file">
    <name>gin_webpimageformat.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_graphics/formats/</path>
    <filename>gin__webpimageformat_8h.html</filename>
    <class kind="class">WEBPImageFormat</class>
  </compound>
  <compound kind="file">
    <name>gin_websocket.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_network/network/</path>
    <filename>gin__websocket_8h.html</filename>
    <class kind="struct">WSHeaderType</class>
    <class kind="class">WebSocket</class>
  </compound>
  <compound kind="file">
    <name>gin_wtoscillators.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/dsp/</path>
    <filename>gin__wtoscillators_8h.html</filename>
    <class kind="class">WTOscillator</class>
    <class kind="struct">WTOscillator::Params</class>
    <class kind="class">WTVoicedStereoOscillator</class>
    <member kind="function">
      <type>bool</type>
      <name>loadWavetables</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga8167e97c7e5fc9916a3423ca494b72b8</anchor>
      <arglist>(juce::OwnedArray&lt; BandLimitedLookupTable &gt; &amp;bllt, juce::AudioSampleBuffer &amp;buffer, double sampleRate, int tableSize)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>gin_xmpmetadata.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_metadata/metadata/</path>
    <filename>gin__xmpmetadata_8h.html</filename>
    <class kind="class">XmpMetadata</class>
  </compound>
  <compound kind="file">
    <name>gin_xyscope.h</name>
    <path>/home/runner/work/Gin/Gin/doxygen/build/gin_dsp/components/</path>
    <filename>gin__xyscope_8h.html</filename>
    <class kind="class">XYScope</class>
  </compound>
  <compound kind="class">
    <name>ADSR</name>
    <filename>classADSR.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>State</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>idle</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aa5ee791507471a07b686dd1b63486b0e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>attack</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aaa7f97349a3590aa462a5edadb7180126</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>decay</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aabc2ac366007251aecdaaff91f4a8e5e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>sustain</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aa48dd9dd380d2a499ad6f7ae967010835</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>release</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aac3273f5775dd2ec96038b66c3909d0a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>idle</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aa5ee791507471a07b686dd1b63486b0e7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>attack</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aaa7f97349a3590aa462a5edadb7180126</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>decay</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aabc2ac366007251aecdaaff91f4a8e5e3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>sustain</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aa48dd9dd380d2a499ad6f7ae967010835</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>release</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ab573baeeb4578e0a1e6e123452b8127aac3273f5775dd2ec96038b66c3909d0a1</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ADSR</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a23264ae8eb79a35c57f648c589b218ae</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ADSR</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ad358afeb54bb48df4f6eb3cbb42bf9ed</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>af6e69c2c20e619a9acf144ac8d24493d</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>process</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a3a1efe888ece1888c0ac82a3ebe18183</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>process</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a81557ac10b1843a41d9a4e402ec11405</anchor>
      <arglist>(int num)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ad8942eb6422a6e5fb6dbaa7fb0f18ec5</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a447f324c6cef2b83bb0858ce449e9bf4</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, int startSample, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getOutput</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>aad2376dfb5d854dd0250e2a6bde52cb2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>State</type>
      <name>getState</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a9b2307478e8638d97ce6fddf2540e9e6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a9d5b7c8618c2c836be04e3549eb19d25</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOff</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>aa187e99749943e75013e40a4e6d4c07e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setAttack</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a94d90ba1cd4fc7e4cf5508e746a7e53e</anchor>
      <arglist>(float seconds)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDecay</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>adb8cfc9ecc4bcfa57dbdcb29dc94f57b</anchor>
      <arglist>(float seconds)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRelease</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>aba5e155a2619b03292ac328732ac679d</anchor>
      <arglist>(float seconds)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSustainLevel</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>af9f4efe95e72ee6c0066a0249794f367</anchor>
      <arglist>(float level)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>aa2f7f151d7baf0051a27cba6682903aa</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>State</type>
      <name>state</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a2d9269b27359df5559e6881e5061e790</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sampleRate</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a836c5dd7fdafc30acccedb6b8346a86a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>output</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ac6b34bdf72ba654d26467433f7015bbb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>attackDelta</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>ae3003291adf6ae972e1ef26ec5f39ad1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>decayDelta</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a7fe4cf55dca362fd768e8763e1207721</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>releaseDelta</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>a667ee4e0401be9138a80249cc45281df</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>sustainLevel</name>
      <anchorfile>classADSR.html</anchorfile>
      <anchor>aa02446c404e961975776b5b7d40d91a7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ADSRComponent</name>
    <filename>classADSRComponent.html</filename>
    <base>MultiParamComponent</base>
    <member kind="function">
      <type></type>
      <name>ADSRComponent</name>
      <anchorfile>classADSRComponent.html</anchorfile>
      <anchor>a3f7c89a9e654aaf93b21febc32df8a69</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ADSRComponent</name>
      <anchorfile>classADSRComponent.html</anchorfile>
      <anchor>a83debe35b04a6d93ffa107e49abe60bd</anchor>
      <arglist>() override=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classADSRComponent.html</anchorfile>
      <anchor>a7ff87752dfde41f395aa294e8ee23568</anchor>
      <arglist>(Parameter::Ptr attack, Parameter::Ptr decay, Parameter::Ptr sustain, Parameter::Ptr release)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AnalogADSR</name>
    <filename>classAnalogADSR.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>State</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>aa8a2577abb5a28fc96d0fd60a3b7188e</anchor>
      <arglist></arglist>
      <enumvalue file="classAnalogADSR.html" anchor="aa8a2577abb5a28fc96d0fd60a3b7188eaec2f993aec2c27fc750119ab17b16cdb">idle</enumvalue>
      <enumvalue file="classAnalogADSR.html" anchor="aa8a2577abb5a28fc96d0fd60a3b7188eafc7e987f23de5bd6562b7c0063cad659">attack</enumvalue>
      <enumvalue file="classAnalogADSR.html" anchor="aa8a2577abb5a28fc96d0fd60a3b7188ea63faabca234a29640f3344e74c0672da">decay</enumvalue>
      <enumvalue file="classAnalogADSR.html" anchor="aa8a2577abb5a28fc96d0fd60a3b7188ea02bc1150d41a4dfdc324849c25dce148">sustain</enumvalue>
      <enumvalue file="classAnalogADSR.html" anchor="aa8a2577abb5a28fc96d0fd60a3b7188ea123fead50246387983ee340507115ef4">release</enumvalue>
    </member>
    <member kind="function">
      <type></type>
      <name>AnalogADSR</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>ac3e0a7c749f0e3f792bec98f11afc36d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~AnalogADSR</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a5afe3f9e7afd79741e8a6ea101aa3a15</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>ae1b202f50148cb7b95e02af59bc1622d</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>process</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a97e522df9b6b57940abae0d6c8a3f0a7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>process</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>ab4b9db12fffa4ec562d44c8f0c8015e4</anchor>
      <arglist>(int num)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>ae60ca06ba8ccf837e4a0322d3983b1c3</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a9e83197b202536d8e29f326c975ff939</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, int startSample, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processMultiplying</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a0fbc0634af4beb8f56840baae672f3ec</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processMultiplying</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a4adbb803653e45ab63ba4fed94ce7719</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, int startSample, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getOutput</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a0fdf4b32574c7a2e276ee6e2d2a7b6d1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>State</type>
      <name>getState</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>aa800869aa2b0f0fef2bac1b428487483</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>ae25730d121f437a88107aec4b8954ced</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOff</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a68fa35850ca4f7d0d329079b990e8b04</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setAttack</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a691f88735822877435feb3639b229c98</anchor>
      <arglist>(float seconds)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDecay</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a2a31f6315bfa1309e75b189db74e317c</anchor>
      <arglist>(float seconds)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRelease</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>ae5ddb6e815c4d8fb7864aa87468e5566</anchor>
      <arglist>(float seconds)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSustainLevel</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a9dc744987060e6db1c57cc35c81d5b0b</anchor>
      <arglist>(float level)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a425950fc964cfd53cab5933c8ef80e96</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>calculateAttack</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a4d534dfdae50b1b15e925eb2c197fd54</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>calculateDecay</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a8f577d26489e90025941bc15d09bd7ad</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>calculateRelease</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a38113e1670f963e703a476dd60f623e4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>State</type>
      <name>state</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a72a052eb2cfe616157e185707982f1db</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sampleRate</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a06be6a28f94dfa97edc308dfbdb1687a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>attack</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>ad78b0e1a8896f9d2c021832fa7517982</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>decay</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a708e8fef77e14886a71939f64d7f99eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>sustain</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a6cd2be4762c1a196e9bfae491d5e2399</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>release</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a5498d865f5874daec968d26562431ae9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>attackCoeff</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a2586227ebaf1e5741159415ada492a6f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>decayCoeff</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a579f3a723cf158c51e851339ab151a43</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>releaseCoeff</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a55792cb40790e2d3675365783643c486</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>attackOffset</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>aee1d18ded5252ac1a9ae7b3009bc21e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>decayOffset</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>af483c3fd72c3c558c140032492ce60cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>releaseOffset</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a5e5463c72a492579721244d6d2ff4274</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>output</name>
      <anchorfile>classAnalogADSR.html</anchorfile>
      <anchor>a15bcc5ef1223a2c4e9f8d3e143009370</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AsyncDownload</name>
    <filename>classAsyncDownload.html</filename>
    <member kind="function">
      <type></type>
      <name>AsyncDownload</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>a15f718f78c88455a85c0c79ff6e8ff37</anchor>
      <arglist>(juce::String url_, std::function&lt; void(AsyncDownload *, juce::MemoryBlock, bool)&gt; cb_, int timeoutMS_=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>AsyncDownload</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>a9dce22dda59d049d43ce4ace85880e01</anchor>
      <arglist>(juce::URL url_, std::function&lt; void(AsyncDownload *, juce::MemoryBlock, bool)&gt; cb_, int timeoutMS_=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~AsyncDownload</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>ad5003941070216619614340a29210f1e</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>run</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>a12f678abf96e0c0a0f988baa076ec253</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>readEntireBinaryStream</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>a6a2cf5b219b333b02b2965dfdafde699</anchor>
      <arglist>(juce::MemoryBlock &amp;destData, bool usePostCommand=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>handleAsyncUpdate</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>a17faaedc4393e528d302dc6073511c4f</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>juce::URL</type>
      <name>url</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>aef49e36d34014f5e3d77c044fd1b47d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(AsyncDownload *, juce::MemoryBlock, bool)&gt;</type>
      <name>cb</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>a917bd54326382853cc85d86d4b94e963</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>timeoutMS</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>a3ec126400198a9fd05346471d00b93d6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>ok</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>ace92411a16d6e4ba9bb7247bea3badad</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::MemoryBlock</type>
      <name>data</name>
      <anchorfile>classAsyncDownload.html</anchorfile>
      <anchor>a65842b5ad253633e5d2d6df4464d5125</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AsyncLambdaValueTreeListener</name>
    <filename>classAsyncLambdaValueTreeListener.html</filename>
    <member kind="function">
      <type></type>
      <name>AsyncLambdaValueTreeListener</name>
      <anchorfile>classAsyncLambdaValueTreeListener.html</anchorfile>
      <anchor>a7902f77301e14c547e97ca4c12310f05</anchor>
      <arglist>(juce::ValueTree &amp;v_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~AsyncLambdaValueTreeListener</name>
      <anchorfile>classAsyncLambdaValueTreeListener.html</anchorfile>
      <anchor>a63db65f7ad15b14ef1552ff0a49e8357</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(const juce::ValueTree &amp;, const juce::Identifier &amp;)&gt;</type>
      <name>onValueTreePropertyChanged</name>
      <anchorfile>classAsyncLambdaValueTreeListener.html</anchorfile>
      <anchor>a44bae2ccef390d4cc7597a14ed0890ad</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(const juce::ValueTree &amp;, const juce::ValueTree &amp;)&gt;</type>
      <name>onValueTreeChildAdded</name>
      <anchorfile>classAsyncLambdaValueTreeListener.html</anchorfile>
      <anchor>a49509f3a601c013ed216d83901e5d5b6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(const juce::ValueTree &amp;, const juce::ValueTree &amp;, int)&gt;</type>
      <name>onValueTreeChildRemoved</name>
      <anchorfile>classAsyncLambdaValueTreeListener.html</anchorfile>
      <anchor>acdb36b902336d9389a06cb2da4418f97</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(const juce::ValueTree &amp;, int, int)&gt;</type>
      <name>onValueTreeChildOrderChanged</name>
      <anchorfile>classAsyncLambdaValueTreeListener.html</anchorfile>
      <anchor>a92e18bd7456866b62b78f040a3ccfe7b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(const juce::ValueTree &amp;)&gt;</type>
      <name>onValueTreeParentChanged</name>
      <anchorfile>classAsyncLambdaValueTreeListener.html</anchorfile>
      <anchor>aebb3264b2330cf9be846a447b105f613</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(const juce::ValueTree &amp;)&gt;</type>
      <name>onValueTreeRedirected</name>
      <anchorfile>classAsyncLambdaValueTreeListener.html</anchorfile>
      <anchor>a34c0f546462a7406ce06d13e4bde8ffd</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AsyncWebsocket</name>
    <filename>classAsyncWebsocket.html</filename>
    <member kind="function">
      <type></type>
      <name>AsyncWebsocket</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>a4f7b218910a6a86ea4cbc2d7e249c47e</anchor>
      <arglist>(const juce::URL url)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~AsyncWebsocket</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>a0a277b2e4494535bff9ba2b323f80c2b</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>connect</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>ac4a1d21499ab2ebc02483485eb0c62b5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>disconnect</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>a6cff60be7eb74cdf9dcc03b488ca0558</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isConnected</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>a0ab724696f1561e21e1f2448dd8f34b8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPingInterval</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>a24f2716444fec081a2e09006f7dad11b</anchor>
      <arglist>(int seconds)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>send</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>af7bc5cc80f7ca6fa2375e4eb4bb489f2</anchor>
      <arglist>(const juce::String &amp;text)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>send</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>aa00a8712f4c2d086edcde02683ee9c4b</anchor>
      <arglist>(const juce::MemoryBlock &amp;binary)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sendPing</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>ac50dd50c0c499cae68510fcfe096e82c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void()&gt;</type>
      <name>onConnect</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>af6b1ef0d8ea2d3584dba94e2d4f397bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void()&gt;</type>
      <name>onDisconnect</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>a914a1081af0cb8231377286e3200ccab</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(const juce::String &amp;)&gt;</type>
      <name>onText</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>aa13c72a5601e9d9d23afb0ff7802ae94</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(const juce::MemoryBlock &amp;)&gt;</type>
      <name>onBinary</name>
      <anchorfile>classAsyncWebsocket.html</anchorfile>
      <anchor>ace1df325c39ecbd3b1db08877e19f632</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AudioEquationParser</name>
    <filename>classAudioEquationParser.html</filename>
    <base>EquationParser</base>
    <base>AudioFunctionHost</base>
    <member kind="function">
      <type></type>
      <name>AudioEquationParser</name>
      <anchorfile>classAudioEquationParser.html</anchorfile>
      <anchor>ad4901750930743500c83acf90d83a7a0</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addConstants</name>
      <anchorfile>classAudioEquationParser.html</anchorfile>
      <anchor>a9b1b040ad25a848fab0cab4d6cceaacc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addUtilities</name>
      <anchorfile>classAudioEquationParser.html</anchorfile>
      <anchor>a7e1106df9e22f1a75e3237733256088d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addOscillatorFunctions</name>
      <anchorfile>classAudioEquationParser.html</anchorfile>
      <anchor>ad45fc68a395dae95ac2e3313bf68d9cd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addSynthFilterFunctions</name>
      <anchorfile>classAudioEquationParser.html</anchorfile>
      <anchor>a54658381420a18612ea9c73fa03c6c8c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addEffectFilterFunctions</name>
      <anchorfile>classAudioEquationParser.html</anchorfile>
      <anchor>af1a46291ba482c1b72ccb44dae1236d7</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AudioFifo</name>
    <filename>classAudioFifo.html</filename>
    <member kind="function">
      <type></type>
      <name>AudioFifo</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a521bc70e223aabb88d2579d44d63d6ec</anchor>
      <arglist>(int channels=2, int numSamples=128)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSize</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>af2bfc029b2b45ca5c33f329cc1c39646</anchor>
      <arglist>(int numChannels, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getFreeSpace</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>af75865aa3db5a5fb798768c6c358d147</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumReady</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a716f99ce4611d9cdaa3f3b10e03dd4ce</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a172d185b091450adae7f12069b8c8541</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>ensureFreeSpace</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a5bad74d132c264adbda154425546944f</anchor>
      <arglist>(int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>write</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a272a4efa8416e0c57905329552781855</anchor>
      <arglist>(const juce::AudioSampleBuffer &amp;src, int numSamples=-1)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>write</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a7dbba1b96c2670e3e046f2ad83e3ed40</anchor>
      <arglist>(const float *const *data, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>writeMono</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a8a2f35a383ee1e3074491dfc6f510e59</anchor>
      <arglist>(const float *data, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>read</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a069465c3976a0dc32e785babe6532898</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;dest)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>read</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a2430de299a81f6ac03cd0200590a455a</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;dest, int startSampleInDestBuffer, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>readMono</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>a8fbc006a7362cca45d85fba09fd5969e</anchor>
      <arglist>(float *data, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>readAdding</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>adba2109354e5ae31b9ea6214512623d1</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;dest)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>readAdding</name>
      <anchorfile>classAudioFifo.html</anchorfile>
      <anchor>ade7b74334094b86983e5deaf2dd11e47</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;dest, int startSampleInDestBuffer, int numSamples)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>AudioFunctionHost</name>
    <filename>classAudioFunctionHost.html</filename>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>a03addbf222d8ebf72559c099c1f711bf</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addConstants</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>a19418afc0b68f06e24d664895ddc4749</anchor>
      <arglist>(gin::EquationParser &amp;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addUtilities</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>aba304cc089ba932e0d926ab18221df21</anchor>
      <arglist>(gin::EquationParser &amp;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addOscillatorFunctions</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>a6f766568fdf3858795232ce98797e58e</anchor>
      <arglist>(gin::EquationParser &amp;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addSynthFilterFunctions</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>a40cd2ad73ee68e4d4cf55753b2686b4c</anchor>
      <arglist>(gin::EquationParser &amp;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addEffectFilterFunctions</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>ad3a16e676235cf5f04ee344fede89247</anchor>
      <arglist>(gin::EquationParser &amp;)</arglist>
    </member>
    <member kind="variable">
      <type>gin::BandLimitedLookupTables *</type>
      <name>lookupTables</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>ac3f36a29508908517af32e673583014e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="protected">
      <type>T *</type>
      <name>getFuncParams</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>a12108539be4ba4eb153718b2a369a098</anchor>
      <arglist>(int i, double sr)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::map&lt; int, std::unique_ptr&lt; FuncState &gt; &gt;</type>
      <name>funcStates</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>aa8b5235e076cae5d2c28443fe8ba1ca5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sampleRate</name>
      <anchorfile>classAudioFunctionHost.html</anchorfile>
      <anchor>a3647d97cb87c9029d48846985dcfe39a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>PatchBrowser::AuthorsModel</name>
    <filename>structPatchBrowser_1_1AuthorsModel.html</filename>
    <member kind="function">
      <type></type>
      <name>AuthorsModel</name>
      <anchorfile>structPatchBrowser_1_1AuthorsModel.html</anchorfile>
      <anchor>a5b1e9a0aee87d3623f78db4126fcab3e</anchor>
      <arglist>(PatchBrowser &amp;o)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumRows</name>
      <anchorfile>structPatchBrowser_1_1AuthorsModel.html</anchorfile>
      <anchor>a6f535e94dba0d2e1592f1bbc891fcacf</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>selectedRowsChanged</name>
      <anchorfile>structPatchBrowser_1_1AuthorsModel.html</anchorfile>
      <anchor>af66565febcb8e763a057222851b72d1e</anchor>
      <arglist>(int) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paintListBoxItem</name>
      <anchorfile>structPatchBrowser_1_1AuthorsModel.html</anchorfile>
      <anchor>a8b7bedbb029b3ba34b13c34a48626eed</anchor>
      <arglist>(int row, juce::Graphics &amp;g, int w, int h, bool selected) override</arglist>
    </member>
    <member kind="variable">
      <type>PatchBrowser &amp;</type>
      <name>owner</name>
      <anchorfile>structPatchBrowser_1_1AuthorsModel.html</anchorfile>
      <anchor>aa70603530cb3faec04b1711a7617e673</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BandLimitedLookupTable</name>
    <filename>classBandLimitedLookupTable.html</filename>
    <member kind="function">
      <type></type>
      <name>BandLimitedLookupTable</name>
      <anchorfile>classBandLimitedLookupTable.html</anchorfile>
      <anchor>ad40d2d7deba27521fda4c55280418943</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>BandLimitedLookupTable</name>
      <anchorfile>classBandLimitedLookupTable.html</anchorfile>
      <anchor>aa8d1104796d5b6ce9103280164f23dab</anchor>
      <arglist>(std::function&lt; double(double, double, double)&gt; function, double sampleRate, int notesPerTable_=6, int tableSize_=2048)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classBandLimitedLookupTable.html</anchorfile>
      <anchor>a3dab85dafee0db1469251e9c59cd22e9</anchor>
      <arglist>(std::function&lt; double(double, double, double)&gt; function, double sampleRate, int notesPerTable_=6, int tableSize=2048)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>process</name>
      <anchorfile>classBandLimitedLookupTable.html</anchorfile>
      <anchor>ae4bbf0cf3a280899c20e09895e509786</anchor>
      <arglist>(float note, float phase)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>loadFromBuffer</name>
      <anchorfile>classBandLimitedLookupTable.html</anchorfile>
      <anchor>a168317b2249b5633e08329eeabd7a09c</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, double sampleRate, int notesPerTable)</arglist>
    </member>
    <member kind="variable">
      <type>juce::OwnedArray&lt; juce::dsp::LookupTableTransform&lt; float &gt; &gt;</type>
      <name>tables</name>
      <anchorfile>classBandLimitedLookupTable.html</anchorfile>
      <anchor>adbe6b6e06f76c87e7bfee87d854b4ffe</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>notesPerTable</name>
      <anchorfile>classBandLimitedLookupTable.html</anchorfile>
      <anchor>a16759d173143073a70627173a7b45220</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BandLimitedLookupTables</name>
    <filename>classBandLimitedLookupTables.html</filename>
    <member kind="function">
      <type></type>
      <name>BandLimitedLookupTables</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>a7e1c8aa33089ca6a78dcfb204e2e916d</anchor>
      <arglist>(double sampleRate=44100, int notesPerTable=3, int tableSize=2048)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>a615ea2571866e5f4c3e97cad98e44c6b</anchor>
      <arglist>(double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>processSine</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>adf067ab0c34d191b5369da60d4586a72</anchor>
      <arglist>(float phase)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>processTriangle</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>a64def1c60b877662f0046a0837d6543e</anchor>
      <arglist>(float note, float phase)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>processSawUp</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>afb69ff9232d47e6c200434fb394fefdd</anchor>
      <arglist>(float note, float phase)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>processSawDown</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>a14edeb68c6de5b718ce9ca07886f8858</anchor>
      <arglist>(float note, float phase)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>processSquare</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>a194da9ad3989b0e8845c16c9792bfe21</anchor>
      <arglist>(float note, float phase)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>processPulse</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>a95aa08a1c4943b1e3ca991ae3316aeba</anchor>
      <arglist>(float note, float phase, float pw)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>process</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>ad49cd21400d1e108e677a10ae5cddeb1</anchor>
      <arglist>(Wave wave, float note, float phase, float pw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getSampleRate</name>
      <anchorfile>classBandLimitedLookupTables.html</anchorfile>
      <anchor>a82d39f6516c342b2ff3e5f08c7be21c3</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BLLTVoicedStereoOscillator</name>
    <filename>classBLLTVoicedStereoOscillator.html</filename>
    <base>VoicedStereoOscillator&lt; StereoOscillator &gt;</base>
    <member kind="function">
      <type></type>
      <name>BLLTVoicedStereoOscillator</name>
      <anchorfile>classBLLTVoicedStereoOscillator.html</anchorfile>
      <anchor>a91580541e11d22946ec60278e65835ea</anchor>
      <arglist>(BandLimitedLookupTables &amp;bllt, int maxVoices=8)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BMPImageFormat</name>
    <filename>classBMPImageFormat.html</filename>
    <member kind="function">
      <type>juce::String</type>
      <name>getFormatName</name>
      <anchorfile>classBMPImageFormat.html</anchorfile>
      <anchor>a2cdb5175be3bb6b90eeb3d6debcce4d1</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>canUnderstand</name>
      <anchorfile>classBMPImageFormat.html</anchorfile>
      <anchor>acad298b2621ad5fbd10b53c3cf85ad84</anchor>
      <arglist>(juce::InputStream &amp;input) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>usesFileExtension</name>
      <anchorfile>classBMPImageFormat.html</anchorfile>
      <anchor>ac3aec001afc89383d536241e433cb8bf</anchor>
      <arglist>(const juce::File &amp;possibleFile) override</arglist>
    </member>
    <member kind="function">
      <type>juce::Image</type>
      <name>decodeImage</name>
      <anchorfile>classBMPImageFormat.html</anchorfile>
      <anchor>a2594f1a9444754276035be54722c59f2</anchor>
      <arglist>(juce::InputStream &amp;input) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>writeImageToStream</name>
      <anchorfile>classBMPImageFormat.html</anchorfile>
      <anchor>a7aa7a97cf8a3e5724bc61550ce7c8881</anchor>
      <arglist>(const juce::Image &amp;sourceImage, juce::OutputStream &amp;destStream) override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BoxArea</name>
    <filename>classBoxArea.html</filename>
    <member kind="function">
      <type></type>
      <name>BoxArea</name>
      <anchorfile>classBoxArea.html</anchorfile>
      <anchor>a00b02a91aded6fc3ead888c1b092a9eb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addBox</name>
      <anchorfile>classBoxArea.html</anchorfile>
      <anchor>a42cd8f8f7bb5274cf58983b056a306c8</anchor>
      <arglist>(Component *c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addBox</name>
      <anchorfile>classBoxArea.html</anchorfile>
      <anchor>ad89bcea639cc032602d1f3e82691f599</anchor>
      <arglist>(int page, Component *c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPage</name>
      <anchorfile>classBoxArea.html</anchorfile>
      <anchor>a7ca0eda894722e20b00cbad62b46791b</anchor>
      <arglist>(int page)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BP12State</name>
    <filename>structBP12State.html</filename>
    <base>FuncState</base>
    <member kind="function">
      <type></type>
      <name>BP12State</name>
      <anchorfile>structBP12State.html</anchorfile>
      <anchor>a117eb714a9f055d4ca05d80c28c10553</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>process</name>
      <anchorfile>structBP12State.html</anchorfile>
      <anchor>a1bd103a04a26c91d07bd05e4a603a787</anchor>
      <arglist>(double v, double freq, double q)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structBP12State.html</anchorfile>
      <anchor>ab6f9745fcf98347271ccc35380e87811</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter</name>
      <anchorfile>structBP12State.html</anchorfile>
      <anchor>a8ba2d9b2b1c8d1fc290894fd796504f7</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BP24State</name>
    <filename>structBP24State.html</filename>
    <base>FuncState</base>
    <member kind="function">
      <type></type>
      <name>BP24State</name>
      <anchorfile>structBP24State.html</anchorfile>
      <anchor>a321d2cbd60a9aeba2ffdb2d54e12c5ea</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>process</name>
      <anchorfile>structBP24State.html</anchorfile>
      <anchor>aeb590c82851a1bb7028b457b29e155fd</anchor>
      <arglist>(double v, double freq, double q)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structBP24State.html</anchorfile>
      <anchor>af10acc0413d14ee09dfc22554037f55b</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter1</name>
      <anchorfile>structBP24State.html</anchorfile>
      <anchor>a7a1b21ac3db5d5807079ffe80ae60c24</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter2</name>
      <anchorfile>structBP24State.html</anchorfile>
      <anchor>a02c9da59417956981fef2420eafcdd7a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>CoalescedTimer</name>
    <filename>classCoalescedTimer.html</filename>
    <member kind="function">
      <type></type>
      <name>CoalescedTimer</name>
      <anchorfile>classCoalescedTimer.html</anchorfile>
      <anchor>ad9d1190d2207b8c651f7daabe749e65e</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~CoalescedTimer</name>
      <anchorfile>classCoalescedTimer.html</anchorfile>
      <anchor>a40c33871fd2db541b880ad506c1dcc0b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>startTimer</name>
      <anchorfile>classCoalescedTimer.html</anchorfile>
      <anchor>a97261711749a65434b7f875f94118c4b</anchor>
      <arglist>(int ms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>startTimerHz</name>
      <anchorfile>classCoalescedTimer.html</anchorfile>
      <anchor>a31d6f6484b273fe894be54c6dcbd0957</anchor>
      <arglist>(int hz)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stopTimer</name>
      <anchorfile>classCoalescedTimer.html</anchorfile>
      <anchor>a80a2fac23a6259ed018d8ba996dd0315</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void()&gt;</type>
      <name>onTimer</name>
      <anchorfile>classCoalescedTimer.html</anchorfile>
      <anchor>afc1e468122b95ea1abf0604c9cf5bfec</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ColourPropertyComponent</name>
    <filename>classColourPropertyComponent.html</filename>
    <base>PropertyComponentBase</base>
    <member kind="function">
      <type></type>
      <name>ColourPropertyComponent</name>
      <anchorfile>classColourPropertyComponent.html</anchorfile>
      <anchor>abc15e762c997ac76cac786201b8db2d2</anchor>
      <arglist>(const juce::Value &amp;valueToControl, const juce::String &amp;propertyName, bool showAlpha=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>refresh</name>
      <anchorfile>classColourPropertyComponent.html</anchorfile>
      <anchor>a1d11ef237f9b2902fcff77bc1c3fea46</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classColourPropertyComponent.html</anchorfile>
      <anchor>a88ecbd7c5a9136910234be51eb487a71</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>CommentMetadata</name>
    <filename>classCommentMetadata.html</filename>
    <base>ImageMetadata</base>
    <member kind="function">
      <type></type>
      <name>CommentMetadata</name>
      <anchorfile>classCommentMetadata.html</anchorfile>
      <anchor>abac88dc934141e297d3c8575e7b99c2e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~CommentMetadata</name>
      <anchorfile>classCommentMetadata.html</anchorfile>
      <anchor>af26102b15ed0813fa97e828f51ef70c5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::StringPairArray</type>
      <name>getAllMetadata</name>
      <anchorfile>classCommentMetadata.html</anchorfile>
      <anchor>a31b960c7e2ed44bc19955db8c75e0edb</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static CommentMetadata *</type>
      <name>create</name>
      <anchorfile>classCommentMetadata.html</anchorfile>
      <anchor>a721c8696bf1f5782eeb683b76ae9d516</anchor>
      <arglist>(const juce::uint8 *data, int sz)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ComponentViewer</name>
    <filename>classComponentViewer.html</filename>
    <member kind="function">
      <type></type>
      <name>ComponentViewer</name>
      <anchorfile>classComponentViewer.html</anchorfile>
      <anchor>ad2606c2be76e8a56bb98d57ce57c0b6f</anchor>
      <arglist>(Component *toTrack=nullptr, juce::PropertiesFile *settings=nullptr, bool alwaysOnTop=true)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ComponentViewer</name>
      <anchorfile>classComponentViewer.html</anchorfile>
      <anchor>a50ad244fd889c8059930d947aa206d48</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void()&gt;</type>
      <name>onClose</name>
      <anchorfile>classComponentViewer.html</anchorfile>
      <anchor>a920864da69c29c1834621727758557c4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ControlBox</name>
    <filename>classControlBox.html</filename>
    <base>MultiParamComponent</base>
    <member kind="function">
      <type></type>
      <name>ControlBox</name>
      <anchorfile>classControlBox.html</anchorfile>
      <anchor>ad4f9fd1d3bd2be33f40ed7bdf1b166c7</anchor>
      <arglist>(ProcessorEditor &amp;e)</arglist>
    </member>
    <member kind="function">
      <type>juce::Rectangle&lt; int &gt;</type>
      <name>getGridArea</name>
      <anchorfile>classControlBox.html</anchorfile>
      <anchor>a3a1c777e4df5d8d4ca16e3b8ffe3798e</anchor>
      <arglist>(int x, int y, int w=1, int h=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classControlBox.html</anchorfile>
      <anchor>a76425dd176246a60018dc6d1da8e65b8</anchor>
      <arglist>(Component *c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classControlBox.html</anchorfile>
      <anchor>a13f5091b0650ef06935392442acbd7c3</anchor>
      <arglist>(int page, ParamComponent *c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPage</name>
      <anchorfile>classControlBox.html</anchorfile>
      <anchor>aa0d350faeed46813d5002a6608452654</anchor>
      <arglist>(int page)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>CopperLookAndFeel</name>
    <filename>classCopperLookAndFeel.html</filename>
    <base>PluginLookAndFeel</base>
    <member kind="function">
      <type></type>
      <name>CopperLookAndFeel</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>af028dd79da3e0bd1d8d52ed602e37008</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::Typeface::Ptr</type>
      <name>getTypefaceForFont</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>a6916406a6f2f350f6d7b348d4e936dbd</anchor>
      <arglist>(const juce::Font &amp;f) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawRotarySlider</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>a635b1059994a6c8bc912a25d4434a39a</anchor>
      <arglist>(juce::Graphics &amp;, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawLinearSlider</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>a76781acc3128172525772d1067b05363</anchor>
      <arglist>(juce::Graphics &amp;, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawButtonBackground</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>a32a1cd825539046b94824d1f3e31a05a</anchor>
      <arglist>(juce::Graphics &amp;, juce::Button &amp;, const juce::Colour &amp;backgroundColour, bool isMouseOverButton, bool isButtonDown) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawButtonText</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>a21381bda8809e601aac6e5c3f83e0a42</anchor>
      <arglist>(juce::Graphics &amp;, juce::TextButton &amp;, bool isMouseOverButton, bool isButtonDown) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawComboBox</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>aaae624d7f83c75904ad61b7774a8b99c</anchor>
      <arglist>(juce::Graphics &amp;, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>positionComboBoxText</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>ab3a0479798b3522015708a40ec018844</anchor>
      <arglist>(juce::ComboBox &amp;, juce::Label &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawTextEditorOutline</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>a6f6210f9d52087f213b1234381c028e7</anchor>
      <arglist>(juce::Graphics &amp;, int width, int height, juce::TextEditor &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>juce::PopupMenu::Options</type>
      <name>getOptionsForComboBoxPopupMenu</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>af423d163ab73fe036783734cbc88e9ae</anchor>
      <arglist>(juce::ComboBox &amp;box, juce::Label &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getAlertWindowButtonHeight</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>a928163d12b9c62bbeac79cc94c0d04e0</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>juce::Font</type>
      <name>getLabelFont</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>a156cada54186dfac49b6e867fa529eaa</anchor>
      <arglist>(juce::Label &amp;) override</arglist>
    </member>
    <member kind="variable">
      <type>juce::SharedResourcePointer&lt; Images &gt;</type>
      <name>images</name>
      <anchorfile>classCopperLookAndFeel.html</anchorfile>
      <anchor>a1515b1d303c786c5721d72b2d25e942d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>CopperLookAndFeelWrapper</name>
    <filename>classCopperLookAndFeelWrapper.html</filename>
    <base>CopperLookAndFeel</base>
    <member kind="function">
      <type></type>
      <name>CopperLookAndFeelWrapper</name>
      <anchorfile>classCopperLookAndFeelWrapper.html</anchorfile>
      <anchor>aed2c58e62600ce32f067bea28c498903</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~CopperLookAndFeelWrapper</name>
      <anchorfile>classCopperLookAndFeelWrapper.html</anchorfile>
      <anchor>af2da6ddd887a26c92e047e30d0f08c9a</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DelayLine</name>
    <filename>classDelayLine.html</filename>
    <member kind="function">
      <type></type>
      <name>DelayLine</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>ad98998dd98c0f9d887c3bdd675dd6e3c</anchor>
      <arglist>(int channels, double maximumDelay=0.001f, double sr=44100.0f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSize</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>ae151b754e94c4ff1a0d1affabce89eeb</anchor>
      <arglist>(int channels, double maximumDelay, double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>ad9772448deb700a0f86f44bf1e4b65a0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>readLinear</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>a2d418fd7a015aa7518c3adf46e978d8e</anchor>
      <arglist>(int ch, double t)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>readSample</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>ad5ba8b3041d885a7cd2f7147b7e59fc6</anchor>
      <arglist>(int ch, int samplePos)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>ae83573274b47e182d7293edd582ae2f3</anchor>
      <arglist>(int ch, float input)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>writeFinished</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>ab36052a31da09d3a8162651ba437bec6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::AudioSampleBuffer</type>
      <name>buffer</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>a93574b33770845363b409a6578919369</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float *const  *</type>
      <name>data</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>a7626cd60f96412594a28f4471337587b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sampleRate</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>a837d7a6d6024a6416ebe97b7b82bd467</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>writePos</name>
      <anchorfile>classDelayLine.html</anchorfile>
      <anchor>a277d65ab60f42d22f55d48596412c897</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Distortion</name>
    <filename>classDistortion.html</filename>
    <member kind="function">
      <type></type>
      <name>Distortion</name>
      <anchorfile>classDistortion.html</anchorfile>
      <anchor>af712b1eb22bc79f96a2febc3e21b859a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classDistortion.html</anchorfile>
      <anchor>ac9e20646253064498bef357d9c06129d</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classDistortion.html</anchorfile>
      <anchor>a119209981bf030951cd9c43ea2d660fb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classDistortion.html</anchorfile>
      <anchor>a79d1cc02cc32a4a891f8aacb2939d9b6</anchor>
      <arglist>(float density, float highpass, float output, float mix)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classDistortion.html</anchorfile>
      <anchor>a28084339a50e41fd194eef4f6e02bda0</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DownloadManager</name>
    <filename>classDownloadManager.html</filename>
    <class kind="struct">DownloadManager::DownloadResult</class>
    <member kind="function">
      <type></type>
      <name>DownloadManager</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a39f2f1950cfb9d84da7f8c73e1207575</anchor>
      <arglist>(int connectTimeout=30 *1000, int shutdownTimeout=30 *1000)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DownloadManager</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>abb67275a951d706a3108a0d307ea9113</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setQueueFinishedCallback</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>ad0366a425102a59335a6cd6eacfb1e07</anchor>
      <arglist>(std::function&lt; void()&gt; callback)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setConnectTimeout</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a16b7b22f909984b2978b841a99f3025d</anchor>
      <arglist>(int timeout)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRetryLimit</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a7e17fbae40ab73beed856f7a28bb2726</anchor>
      <arglist>(int limit)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRetryDelay</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a9911ed025811e16b55a176112f2c8320</anchor>
      <arglist>(double seconds)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setConcurrentDownloadLimit</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a51c68cae2cd149ad036d5124fed18da8</anchor>
      <arglist>(int l)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumberOfDownloads</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>af9024fa154ed3cab25a9f720a0f16bc7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setThreadPriority</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>aa48b0c65fd6624a2479dd5a09845640d</anchor>
      <arglist>(juce::Thread::Priority p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setProgressInterval</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>ab679c640d8a1c5a83a69ccdcb3f145a5</anchor>
      <arglist>(int ms)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDownloadBlockSize</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a9d509b03a25c86d69b410ad99e80cd70</anchor>
      <arglist>(int bs)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumDownloadsInQueue</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a9f504d3d1f26d2c4d390178b04db4d87</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>enableGzipDeflate</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a000ad4728048d97e648423b89447f18e</anchor>
      <arglist>(bool e)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pauseDownloads</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a736bec2349be86541095fe82a1916dbe</anchor>
      <arglist>(bool)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>startAsyncDownload</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a5f28dbb10b11644ea9033f2c4fb2b6ca</anchor>
      <arglist>(juce::String url, juce::String postData, std::function&lt; void(DownloadResult)&gt; completionCallback, std::function&lt; void(juce::int64, juce::int64, juce::int64)&gt; progressCallback=nullptr, juce::String extraHeaders={})</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>startAsyncDownload</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a6010c1da81e49143c4f97a923d056ecb</anchor>
      <arglist>(juce::URL url, std::function&lt; void(DownloadResult)&gt; completionCallback, std::function&lt; void(juce::int64, juce::int64, juce::int64)&gt; progressCallback=nullptr, juce::String extraHeaders={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cancelAllDownloads</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>aecbecc0c8aa4113566906e816eeca7c3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cancelDownload</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>ab1740214c8eb0c508f4e3a48ba394a1c</anchor>
      <arglist>(int downloadId)</arglist>
    </member>
    <member kind="function">
      <type>DownloadResult</type>
      <name>blockingDownload</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>a46d6fd6994ca4cdcde6564edd621020f</anchor>
      <arglist>(juce::String url, juce::String postData, juce::String extraHeaders={})</arglist>
    </member>
    <member kind="function">
      <type>DownloadResult</type>
      <name>blockingDownload</name>
      <anchorfile>classDownloadManager.html</anchorfile>
      <anchor>ab352a3c9d99319f4872c1603eb19cc78</anchor>
      <arglist>(juce::URL url, juce::String extraHeaders={})</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>DownloadManager::DownloadResult</name>
    <filename>structDownloadManager_1_1DownloadResult.html</filename>
    <member kind="function">
      <type>bool</type>
      <name>saveToFile</name>
      <anchorfile>structDownloadManager_1_1DownloadResult.html</anchorfile>
      <anchor>addd2bcdfcf44b5d58f9d5ee8bd148fe7</anchor>
      <arglist>(const juce::File &amp;file, bool overwrite=true)</arglist>
    </member>
    <member kind="variable">
      <type>juce::URL</type>
      <name>url</name>
      <anchorfile>structDownloadManager_1_1DownloadResult.html</anchorfile>
      <anchor>a700f6508383b439c9f700ee6a2193545</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>downloadId</name>
      <anchorfile>structDownloadManager_1_1DownloadResult.html</anchorfile>
      <anchor>ae4aee674b27ea37f9af6ccecf43a03a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>attempts</name>
      <anchorfile>structDownloadManager_1_1DownloadResult.html</anchorfile>
      <anchor>a639a9c5b7e7f3b4889a5f53b23e8bc9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::MemoryBlock</type>
      <name>data</name>
      <anchorfile>structDownloadManager_1_1DownloadResult.html</anchorfile>
      <anchor>a13fd2f0e9b09ade1a1b2ccf97c920bf5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>ok</name>
      <anchorfile>structDownloadManager_1_1DownloadResult.html</anchorfile>
      <anchor>ae2a74f1ceab5b5728458b39f09d27a27</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>httpCode</name>
      <anchorfile>structDownloadManager_1_1DownloadResult.html</anchorfile>
      <anchor>a70931a9fd508ce4cc0344be6514aeffb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::StringPairArray</type>
      <name>responseHeaders</name>
      <anchorfile>structDownloadManager_1_1DownloadResult.html</anchorfile>
      <anchor>a48b192b45df8e978f398483f24e38fa2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Dynamics</name>
    <filename>classDynamics.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a6b3502e2f88d8fd32b07239376f19576</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>compressor</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a6b3502e2f88d8fd32b07239376f19576a05d3a93d33e9c50855b28f21a5e3dd4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>limiter</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a6b3502e2f88d8fd32b07239376f19576af7eeb3fb00381e25b0040685eb2a5174</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>expander</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a6b3502e2f88d8fd32b07239376f19576a86eafff6a891eb381e162e7adc73ff7e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>gate</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a6b3502e2f88d8fd32b07239376f19576a5cddded513bf6aca23ba76ef7d2e537f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>compressor</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a6b3502e2f88d8fd32b07239376f19576a05d3a93d33e9c50855b28f21a5e3dd4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>limiter</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a6b3502e2f88d8fd32b07239376f19576af7eeb3fb00381e25b0040685eb2a5174</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>expander</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a6b3502e2f88d8fd32b07239376f19576a86eafff6a891eb381e162e7adc73ff7e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>gate</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a6b3502e2f88d8fd32b07239376f19576a5cddded513bf6aca23ba76ef7d2e537f</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Dynamics</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a95fd017d51604db8690a4bbe8ec74f53</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Dynamics</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>acff60118f6e8bed2bc100b3fd364b367</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a140a20003797d24372d0ac3aeb546d1d</anchor>
      <arglist>(double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setNumChannels</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>adfe378f9283b6ff12b1eab43ef2e1097</anchor>
      <arglist>(int ch)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMode</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>abb7d2814c89e5a5ca3e7066c03ed9c66</anchor>
      <arglist>(Type t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLinked</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a3df98d57afadc92a521bea6c69ee200d</anchor>
      <arglist>(bool l)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a72cb7558a8113bc03168fcf5f012787f</anchor>
      <arglist>(float attackS, float holdS, float releaseS, float thresh, float ratio, float kneeWidth)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setInputGain</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a8b5e1b91c220eb50e4e68fc6c83282df</anchor>
      <arglist>(float g)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOutputGain</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>ae7ad148c7049b91f76262e901f10a31a</anchor>
      <arglist>(float g)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a63d8f557fa562613747ca4eb5ed2e8fd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a10395d0e46c4387190b783c0090545c2</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, juce::AudioSampleBuffer *envelopeOut=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>const LevelTracker &amp;</type>
      <name>getInputTracker</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>ab10ed8398c8ae7de1d91a49f8e374694</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const LevelTracker &amp;</type>
      <name>getOutputTracker</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>ab8ffe164305688efeb5310d97e25fc74</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const LevelTracker &amp;</type>
      <name>getReductionTracker</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a530bce32f3097ec92e28547dc09130e2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>calcCurve</name>
      <anchorfile>classDynamics.html</anchorfile>
      <anchor>a66be96750fe78c9034b88f8b4fdc9380</anchor>
      <arglist>(float detectorValue)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>DynamicsMeter</name>
    <filename>classDynamicsMeter.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ColourIds</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a3f0cee797b7626c9c6486190b95adb5d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a3f0cee797b7626c9c6486190b95adb5da6744a81ab6cbfffbff00d1ce68a85534</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a3f0cee797b7626c9c6486190b95adb5daf1d34747a651ddb124b66daf13337b62</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>meterColourId</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a3f0cee797b7626c9c6486190b95adb5da3910c97ce05f1f60c867d0253d5784ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a3f0cee797b7626c9c6486190b95adb5da6744a81ab6cbfffbff00d1ce68a85534</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a3f0cee797b7626c9c6486190b95adb5daf1d34747a651ddb124b66daf13337b62</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>meterColourId</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a3f0cee797b7626c9c6486190b95adb5da3910c97ce05f1f60c867d0253d5784ba</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>DynamicsMeter</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a3a8565a9c8e14710fcd6ecfe2bcd10e5</anchor>
      <arglist>(Dynamics &amp;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~DynamicsMeter</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>abfd5413d8c210e1fb65b4e257da4335d</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRange</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a0d1820a8f792dad369d386b7050eb268</anchor>
      <arglist>(juce::NormalisableRange&lt; float &gt; r)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a932a59881c3775e70cfdd0870753a897</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>timerCallback</name>
      <anchorfile>classDynamicsMeter.html</anchorfile>
      <anchor>a26663940a83484b73a3aa0c6c436535e</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EasedValueSmoother</name>
    <filename>classEasedValueSmoother.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>af3e8fdfbc38ae23ac474d8b6fae49035</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTime</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>af357f90cafbcd77ed26973f3cfe7a569</anchor>
      <arglist>(double t)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isSmoothing</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>aed083f859f945a33ca3398d5d651feb2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>a206d1556877ab9c11b43e17f5976e6ab</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getCurrentValue</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>a73fa9f74e2e75fbd0d5fcf15d2cbef7d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>T *</type>
      <name>getValuePtr</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>a0c0bd355c729547333266dfe4be7ac18</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>snapToValue</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>a29a41dfa10ea500d9ed4f4e7b45a989f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>a411637268d7606fb829f3b579ddce53c</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>a26b174548959f6d55f52dd1bc9ff6afa</anchor>
      <arglist>(T v)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateValue</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>a266f723fcb2c393fbdd93599d46abcce</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getNextValue</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>abbebcf9bc5851c6bde91969b87aafe7b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValueUnsmoothed</name>
      <anchorfile>classEasedValueSmoother.html</anchorfile>
      <anchor>ae5e2dc56a32512d27b6ac147ae12859f</anchor>
      <arglist>(T v)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Spline::Element</name>
    <filename>classSpline_1_1Element.html</filename>
    <member kind="function">
      <type></type>
      <name>Element</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>afdb3184c95f7b19a407ee422d211cc92</anchor>
      <arglist>(double x_=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Element</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>a597ebfc19202d8bbbd30470a2e5f3860</anchor>
      <arglist>(double x_, double a_, double b_, double c_, double d_)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>eval</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>ab898da37ebf3cd04320cfb9c9266f72e</anchor>
      <arglist>(double xx) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>a10d65f6538da41f43a1b7cd3e47d2a30</anchor>
      <arglist>(const Element &amp;e) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>a6ddfd19f2d38ec958d5c0965797c699e</anchor>
      <arglist>(const double xx) const</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>x</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>aee52a511238fa599c18acede2e338e09</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>a</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>aa20a7343d5bfabf930f48bd5e88d9916</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>b</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>aa17608c49869d7129452fd777f39ee5c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>c</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>aafdf5d207e476f90bb5cda88565de1bb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>d</name>
      <anchorfile>classSpline_1_1Element.html</anchorfile>
      <anchor>aa152a2b6876477cb9945a1de6860f06e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ElevatedFileCopy</name>
    <filename>classElevatedFileCopy.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Result</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a32c2bb5b993a24db8a21295cf2ceb30f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>success</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a32c2bb5b993a24db8a21295cf2ceb30fac98012252d82fd16e8993a48798082da</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>failed</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a32c2bb5b993a24db8a21295cf2ceb30fa765c9be4013c90f5b9658ce40d293c4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>cancelled</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a32c2bb5b993a24db8a21295cf2ceb30faa6801c470e393663100fb049d6b195e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>nopermissions</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a32c2bb5b993a24db8a21295cf2ceb30fabe61a1bc08244491bfbe19296953ddb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>success</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a32c2bb5b993a24db8a21295cf2ceb30fac98012252d82fd16e8993a48798082da</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>failed</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a32c2bb5b993a24db8a21295cf2ceb30fa765c9be4013c90f5b9658ce40d293c4c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>cancelled</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a32c2bb5b993a24db8a21295cf2ceb30faa6801c470e393663100fb049d6b195e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>nopermissions</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a32c2bb5b993a24db8a21295cf2ceb30fabe61a1bc08244491bfbe19296953ddb1</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>createDir</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>adfdda510a8b38c0884eafc83b9ee847a</anchor>
      <arglist>(const juce::File &amp;dir)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copyFile</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a72d1a5cdf30ac2bb43d89934ec7b14f0</anchor>
      <arglist>(const juce::File &amp;src, const juce::File &amp;dst)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>moveFile</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a985419d76baa5bb6cd7def8472a65b1d</anchor>
      <arglist>(const juce::File &amp;src, const juce::File &amp;dst)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>deleteFile</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a388cf3a1477bb16bb43e7e1666c4e46d</anchor>
      <arglist>(const juce::File &amp;f)</arglist>
    </member>
    <member kind="function">
      <type>Result</type>
      <name>execute</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>ac6965dae9ef1cf89d3ebe038d32370e3</anchor>
      <arglist>(bool launchSelf=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a1b6d64bc3b7847628f35c14b9e3c58a8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>processCommandLine</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a105805b9b82a4d0050bcb9804bc72857</anchor>
      <arglist>(juce::String commandLine)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static Result</type>
      <name>runScriptWithAdminAccess</name>
      <anchorfile>classElevatedFileCopy.html</anchorfile>
      <anchor>a38602e1c7ac1fa9d1c57ac467b8fe7bb</anchor>
      <arglist>(juce::File script, bool launchSelf)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Ellipse</name>
    <filename>classEllipse.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>Ellipse</name>
      <anchorfile>classEllipse.html</anchorfile>
      <anchor>ae3280925b24d4858639a7ede8007150f</anchor>
      <arglist>(T a_, T b_)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isPointOn</name>
      <anchorfile>classEllipse.html</anchorfile>
      <anchor>a899aa09f95707ef4d3982d9a3fbe800a</anchor>
      <arglist>(Point&lt; T &gt; pt, T accuracy=0.00001)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isPointOutside</name>
      <anchorfile>classEllipse.html</anchorfile>
      <anchor>a7f3e736575ae9eab8c76664331b98d53</anchor>
      <arglist>(Point&lt; T &gt; pt)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isPointInside</name>
      <anchorfile>classEllipse.html</anchorfile>
      <anchor>a671db8ce37e93c7f47d5bbf440393a51</anchor>
      <arglist>(Point&lt; T &gt; pt)</arglist>
    </member>
    <member kind="function">
      <type>Point&lt; T &gt;</type>
      <name>pointAtAngle</name>
      <anchorfile>classEllipse.html</anchorfile>
      <anchor>a90e578ea5cd1df558af9397e92d3e1e6</anchor>
      <arglist>(T angle)</arglist>
    </member>
    <member kind="variable">
      <type>T</type>
      <name>a</name>
      <anchorfile>classEllipse.html</anchorfile>
      <anchor>a5fb56e7b1960b8e127deccbc91f3aa8d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>T</type>
      <name>b</name>
      <anchorfile>classEllipse.html</anchorfile>
      <anchor>a9b90b5b9f70e55aed03184d13cdea464</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EnvelopeDetector</name>
    <filename>classEnvelopeDetector.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Mode</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aed1a748db06a14d27716c88b0fa1bdb4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>peak</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aed1a748db06a14d27716c88b0fa1bdb4ab6748369a0be703f089ba6dc60b0e91b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ms</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aed1a748db06a14d27716c88b0fa1bdb4a9d8d4339f5a98d805053183b2c70896f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>rms</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aed1a748db06a14d27716c88b0fa1bdb4aa63705cf10474e3baa09f52c7f5bf5e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>peak</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aed1a748db06a14d27716c88b0fa1bdb4ab6748369a0be703f089ba6dc60b0e91b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ms</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aed1a748db06a14d27716c88b0fa1bdb4a9d8d4339f5a98d805053183b2c70896f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>rms</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aed1a748db06a14d27716c88b0fa1bdb4aa63705cf10474e3baa09f52c7f5bf5e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EnvelopeDetector</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a06b5124f6743035636f8c00426b3b81c</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~EnvelopeDetector</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aaffaed493523a4e04dc15798879c95ef</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a2c151e5f1f29e3d901fb9f2dc28a6002</anchor>
      <arglist>(double f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a983d59fabbec5a3ebb64e98be0610520</anchor>
      <arglist>(float attackS, float holdS, float releaseS, bool analogTC, Mode detect, bool logDetector)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a21c729fbcbdac9e36b47144fada3cdda</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>process</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a7baa6c81b6b27cc74dba98a16232a021</anchor>
      <arglist>(float input)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>setAttackTime</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>ac4ad7feba40482f66ba2760596c488c5</anchor>
      <arglist>(float attackS)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>setHoldTime</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a7aef834128e7c2772a5f40546edb3ae2</anchor>
      <arglist>(float holdS)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>setReleaseTime</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aa57e83a330148732fcc170e086077cdb</anchor>
      <arglist>(float releaseS)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sampleRate</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>abe9c3bdb279f4724032f4785b5c77d74</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Mode</type>
      <name>mode</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a1804f11c1918ee0aeee94093a3057415</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>attackTime</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a0e02ce9c360c18f16f656dd5ff8a0f9d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>releaseTime</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>ac3265a876b56e1206513591f5c35ec2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>envelope</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a6cb63a505870b8c7f21f963163341a51</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>holdTime</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a92145a868d64b608b11903a79866ab8c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>holdRemaining</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a45d8c4b34051590225abaf3d766a4da1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>analogTC</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>aef1cbe13f909298336ff3e307e6b252c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>logDetector</name>
      <anchorfile>classEnvelopeDetector.html</anchorfile>
      <anchor>a9b42cde52cbdcb05cbc85a65ba6a017f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EQ</name>
    <filename>classEQ.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>afd7d7b03b08fd8ca89204c04ff847010</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lowshelf</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>afd7d7b03b08fd8ca89204c04ff847010ab5975e5df9ac7649fff9b46aa830aa46</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highshelf</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>afd7d7b03b08fd8ca89204c04ff847010af9006159cc7c1e1041c5206349d39c41</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>peak</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>afd7d7b03b08fd8ca89204c04ff847010a63f376f9d83f0634b8a03016b8fef4c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lowshelf</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>afd7d7b03b08fd8ca89204c04ff847010ab5975e5df9ac7649fff9b46aa830aa46</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highshelf</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>afd7d7b03b08fd8ca89204c04ff847010af9006159cc7c1e1041c5206349d39c41</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>peak</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>afd7d7b03b08fd8ca89204c04ff847010a63f376f9d83f0634b8a03016b8fef4c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EQ</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>a9811cad866d78bf11f43f0afa4f8f03e</anchor>
      <arglist>(int numBands)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>a7392e0d8035979f299cff14da78c949d</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setNumChannels</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>aa35ca943354765d21c025731ef15a858</anchor>
      <arglist>(int ch)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>a5aea12e151ab6d28f87a4c898c24abaf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>a43ae876271c57c0f47b0ee4aa07d98a9</anchor>
      <arglist>(int band, Type t, float freq, float q, float g)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classEQ.html</anchorfile>
      <anchor>ae8e996d04aaebc129a1e66522ce5d379</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EquationParser</name>
    <filename>classEquationParser.html</filename>
    <member kind="function">
      <type></type>
      <name>EquationParser</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a79be05f577534b4ce4b414cefae6f182</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EquationParser</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a0f57c565cbaa777ce2be4fca4dcba31d</anchor>
      <arglist>(juce::String equation)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~EquationParser</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a39e3f9e4335c9f0adae9df76fcbfb0a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setEquation</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>aa6d133fbe94ce08f2d3b751630f3f2ef</anchor>
      <arglist>(juce::String equation)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearVariables</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>ac5e970f7cce272259c829ba072b74962</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addVariable</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>aebf7f4cdf88caa17a85dd4a137b35150</anchor>
      <arglist>(juce::String name, double *value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setVarFactory</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a36b96c894f2d1b4d20f7b11ee018defb</anchor>
      <arglist>(std::function&lt; double *(const char *)&gt; fun)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearConstants</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a82b1caf0b52884d713ddb76990bc93cc</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addConstant</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a8ed6df6210bd739d2001adf382ac56b2</anchor>
      <arglist>(juce::String name, double value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addFunction</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>ad1764b694dc81c8c5b4c5eaf34ded9fa</anchor>
      <arglist>(juce::String name, std::function&lt; double(int id, const juce::String &amp;)&gt; fun)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addFunction</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a5090c82a3374dff11d9552371a2a8659</anchor>
      <arglist>(juce::String name, std::function&lt; double(int id)&gt; fun)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addFunction</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a2bd775e9ac1d42aa5f2f02a64309c27e</anchor>
      <arglist>(juce::String name, std::function&lt; double(int id, double)&gt; fun)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addFunction</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a371eaf5fa5cc15039f9db987d994dae4</anchor>
      <arglist>(juce::String name, std::function&lt; double(int id, double, double)&gt; fun)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addFunction</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>ab229a4424b5bb0c66d40e9522119fe43</anchor>
      <arglist>(juce::String name, std::function&lt; double(int id, double, double, double)&gt; fun)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addFunction</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a401c9d12b506b4acb9a0402d28eb38cf</anchor>
      <arglist>(juce::String name, std::function&lt; double(int id, double, double, double, double)&gt; fun)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>evaluate</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a920e54171a2af2a10c15ea417250db3b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasError</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a267ea42024ab4404885d6549c46b5298</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getError</name>
      <anchorfile>classEquationParser.html</anchorfile>
      <anchor>a8a6c619e84dea76c15b4caa29c09e7c8</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ExifMetadata</name>
    <filename>classExifMetadata.html</filename>
    <base>ImageMetadata</base>
    <member kind="function">
      <type></type>
      <name>ExifMetadata</name>
      <anchorfile>classExifMetadata.html</anchorfile>
      <anchor>afa3a3cefbb26ea66f049bfc49456def9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~ExifMetadata</name>
      <anchorfile>classExifMetadata.html</anchorfile>
      <anchor>a33f81ea135c7a88e16731c4f16cb9450</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::StringPairArray</type>
      <name>getAllMetadata</name>
      <anchorfile>classExifMetadata.html</anchorfile>
      <anchor>a934b7f4952ca5316d089e9230d599165</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>juce::Image</type>
      <name>getThumbnailImage</name>
      <anchorfile>classExifMetadata.html</anchorfile>
      <anchor>a826de9a748dd021a5b1b984e87e35cf7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static ExifMetadata *</type>
      <name>create</name>
      <anchorfile>classExifMetadata.html</anchorfile>
      <anchor>a3f6329e2c6cb7364c551d656c33ac3e2</anchor>
      <arglist>(const juce::uint8 *data, int sz)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>FilePropertyComponent</name>
    <filename>classFilePropertyComponent.html</filename>
    <base>PropertyComponentBase</base>
    <member kind="function">
      <type></type>
      <name>FilePropertyComponent</name>
      <anchorfile>classFilePropertyComponent.html</anchorfile>
      <anchor>a95373d1491b4b24e514f80c806320f56</anchor>
      <arglist>(const juce::Value &amp;valueToControl, const juce::String &amp;propertyName, const juce::String &amp;title_=&quot;Open&quot;, const juce::String pattern_=&quot;*.*&quot;)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>refresh</name>
      <anchorfile>classFilePropertyComponent.html</anchorfile>
      <anchor>a365e42d9718bbabf5b2db953d6de14e2</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>FileSystemWatcher</name>
    <filename>classFileSystemWatcher.html</filename>
    <class kind="class">FileSystemWatcher::Listener</class>
    <member kind="enumeration">
      <type></type>
      <name>FileSystemEvent</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileCreated</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7da8935e36db30f2b16435fe603320a241b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileDeleted</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7da429bef88054b2dd0618102e1abf4f150</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileUpdated</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7dabc075ff82423a1f48c1043f2e8383acf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileRenamedOldName</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7da5a58eb5cd4c39c2655668fc1d3241955</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileRenamedNewName</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7da41c22f8136f11c19e362685eb6d897f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileCreated</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7da8935e36db30f2b16435fe603320a241b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileDeleted</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7da429bef88054b2dd0618102e1abf4f150</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileUpdated</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7dabc075ff82423a1f48c1043f2e8383acf</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileRenamedOldName</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7da5a58eb5cd4c39c2655668fc1d3241955</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>fileRenamedNewName</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a05e51f587cf359c531e227451ad08f7da41c22f8136f11c19e362685eb6d897f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>FileSystemWatcher</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a588829cfbf7981cbd745fc1db5244d86</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~FileSystemWatcher</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>adfdbffe1e371260804159964e919b78b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addFolder</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a90c382d8d77550e122c03dff90a10c4e</anchor>
      <arglist>(const juce::File &amp;folder)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeFolder</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a4a00ea52d73e860fa8396e98b60157f4</anchor>
      <arglist>(const juce::File &amp;folder)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeAllFolders</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a0ad340eac72f4f3c62a6f3b38e231109</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::Array&lt; juce::File &gt;</type>
      <name>getWatchedFolders</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>aee9da6c13de3dfb36ebafc290a0a6b08</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addListener</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>ad2486d627a8a5cc3512e4a47e7a5cdb0</anchor>
      <arglist>(Listener *newListener)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeListener</name>
      <anchorfile>classFileSystemWatcher.html</anchorfile>
      <anchor>a2eac16aab60dc70dff0eb5b7573e250c</anchor>
      <arglist>(Listener *listener)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Filter</name>
    <filename>classFilter.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>none</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0ae6e303ddb4b9092543178146dfe3c9dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lowpass</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0a3bc93a4b236649ec0f71b600fd7c3d5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highpass</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0a4118e76c4cd96f521d5a2aada176e72a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>bandpass</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0a586918b0c5057db76bb06d493a4870d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>notch</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0a0a3bfe9ecaa7b5164c16cbb43457a778</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lowshelf</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0aa16a5d1831ce8ad6cbfe8178c5700d23</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highshelf</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0aa55b7fdb404e127357a11995f063cbee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>peak</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0ad58dea6810feaf18fe38771eae09e203</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>Slope</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a5ef1acf81c79c0c8a0ecd8a52e38d4f2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>db12</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a5ef1acf81c79c0c8a0ecd8a52e38d4f2a2c5e517e0cff208de5a5b332964c98c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>db24</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a5ef1acf81c79c0c8a0ecd8a52e38d4f2a96cc76f4311546df810c13784c2a591a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>none</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0ae6e303ddb4b9092543178146dfe3c9dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lowpass</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0a3bc93a4b236649ec0f71b600fd7c3d5f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highpass</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0a4118e76c4cd96f521d5a2aada176e72a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>bandpass</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0a586918b0c5057db76bb06d493a4870d1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>notch</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0a0a3bfe9ecaa7b5164c16cbb43457a778</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lowshelf</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0aa16a5d1831ce8ad6cbfe8178c5700d23</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highshelf</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0aa55b7fdb404e127357a11995f063cbee</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>peak</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a8f99602312ccfbe9e1598532158726c0ad58dea6810feaf18fe38771eae09e203</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>db12</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a5ef1acf81c79c0c8a0ecd8a52e38d4f2a2c5e517e0cff208de5a5b332964c98c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>db24</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a5ef1acf81c79c0c8a0ecd8a52e38d4f2a96cc76f4311546df810c13784c2a591a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Filter</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>ad15994c30d497afd567a6445446a249e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a545b26e745e3dae6f806a1ae9d522aed</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setNumChannels</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>ab4826bc2ddff32cb9fcb052b9c3d0629</anchor>
      <arglist>(int ch)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setType</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>aa63924d0bce0ebe96ab635d51d4cea1f</anchor>
      <arglist>(Type t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSlope</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>aca9f4becd196a00f918ec1278cd2d9cb</anchor>
      <arglist>(Slope s)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>aea4a28f5eaf6264a5732ed0d6730fa20</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getFrequency</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>ab5da0bdb9d337ee58d0066fb0eff8f80</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>ab874606abc9fd6094ce7b57c81431737</anchor>
      <arglist>(float freq_, float q_, float g_=0.0f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classFilter.html</anchorfile>
      <anchor>a6bb6a7b54fae46ed0a7bade03fc33430</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>FuncState</name>
    <filename>structFuncState.html</filename>
    <member kind="function">
      <type></type>
      <name>FuncState</name>
      <anchorfile>structFuncState.html</anchorfile>
      <anchor>a1a37453451c69ef86bd1c1f07d970c81</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~FuncState</name>
      <anchorfile>structFuncState.html</anchorfile>
      <anchor>af55be56146226b4f859919b1e4041893</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setSampleRate</name>
      <anchorfile>structFuncState.html</anchorfile>
      <anchor>a894b56366072ca75e574057f117b3145</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>reset</name>
      <anchorfile>structFuncState.html</anchorfile>
      <anchor>a1bfb09b6c7022ac3129df85ef89809a8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>double</type>
      <name>sampleRate</name>
      <anchorfile>structFuncState.html</anchorfile>
      <anchor>a0679db11d8cf803ffe8c40b22fcb6fb1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GainProcessor</name>
    <filename>classGainProcessor.html</filename>
    <member kind="function">
      <type>void</type>
      <name>setGain</name>
      <anchorfile>classGainProcessor.html</anchorfile>
      <anchor>a4592a4fb49ec498380748764a879b523</anchor>
      <arglist>(float g)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classGainProcessor.html</anchorfile>
      <anchor>a2210200aa0c57cb9fe8182d160c1326a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classGainProcessor.html</anchorfile>
      <anchor>a02622b9c864ef8b8534e87d242e6ba2b</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GateEffect</name>
    <filename>classGateEffect.html</filename>
    <member kind="function">
      <type></type>
      <name>GateEffect</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a30931317a7688adf95aa1246b65d2bec</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>ae8b56a0b2040a28d36947dd41cf7f252</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>abb38c635bb08f5cd027516f2e9bb9481</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a377d75486bc98070c5e166ae20f42cf5</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, int noteOnSample, int noteOffSample)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLength</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>aff46f98ed7e2cf06d7b3bf5d1f2345a0</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFrequency</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a3706d31bd1758936f8670b78d2feeccb</anchor>
      <arglist>(float f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setStep</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a01ddeaddc9d55e6f8a4229a0e761541c</anchor>
      <arglist>(int step, bool l, bool r)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setAttack</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a90d83f8d8b8a1e2b6dedfcf171368178</anchor>
      <arglist>(float seconds)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRelease</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a87a565280dfffc37cdca298b07cbd266</anchor>
      <arglist>(float seconds)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sampleRate</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a732b0b2be932001f7facf338ad84a2fc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>AnalogADSR</type>
      <name>adsrL</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>af0a57f0f38fd5c8ee5ae1116c155a78e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>AnalogADSR</type>
      <name>adsrR</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>abfb5044f7e394d996ca25ffdaa67b2af</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::Array&lt; bool &gt;</type>
      <name>stepsL</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a29e08c321ed951a7ebee0701cede0ecb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::Array&lt; bool &gt;</type>
      <name>stepsR</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a8967c7224813739a9388d21bd4e177e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>frequency</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>ac66d3981997f43a37900bd2e549b083c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>phase</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>ad44a38a544110cf0a65b7f1c02de768e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>triggered</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a10a311a7ed46c3c2e20a243a359b6da3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>lastLOn</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>acbc162981c0aff17027d3bb1dba2e559</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>lastROn</name>
      <anchorfile>classGateEffect.html</anchorfile>
      <anchor>a22b6b9beb3b81d68b875d4a0e9daa035</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GateEffectComponent</name>
    <filename>classGateEffectComponent.html</filename>
    <base>MultiParamComponent</base>
    <member kind="function">
      <type></type>
      <name>GateEffectComponent</name>
      <anchorfile>classGateEffectComponent.html</anchorfile>
      <anchor>afa777957bcfd629d2a075905db72607a</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~GateEffectComponent</name>
      <anchorfile>classGateEffectComponent.html</anchorfile>
      <anchor>a997c2ba6c8aaa738ff5705e4e8588d91</anchor>
      <arglist>() override=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classGateEffectComponent.html</anchorfile>
      <anchor>ae7dfc1463aceb050c5e522ded20b4c66</anchor>
      <arglist>(Parameter::Ptr length, Parameter::Ptr *l, Parameter::Ptr *r, Parameter::Ptr enable)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GinLookAndFeel</name>
    <filename>classGinLookAndFeel.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ColourIds</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>whiteColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a73a50ed25cfa50172870d472227fc137</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>blackColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6ac7411db48774329c05a27e414ee24595</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>displayColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6aa7af29d3637768fbf97690f5a7d59e5c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6aa4bd0ee587cfbfef51b3b25d622b8a48</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>groupColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6ab08bc4b9b520e583faed901e1e76e59e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lightGroupColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a2864996ef37b5bf663448c3f8ae6f6a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>grey30ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a8836ddb4187fd9683c6952817859a255</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>grey45ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a66a982d75840d72c1c9ef4eb0a302fd6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>grey60ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a87882c3f6219ea41cd1e0522707f4aa5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>grey90ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a7b989dd098f7a0747c523071621e4766</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>glass1ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a36769c6185d02150c1c87ffa812cac37</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>glass2ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a2d85184728940d03f032a57e5b6915f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>matte1ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a9574cb8d76e0b587045e42ba6f560e3e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>matte2ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a92300711403ae30883fac16299863490</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>title1ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a48e1027db85fa69981be45218bd7e6d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>title2ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a539354805ecf1dd789adb31680e32326</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>accentColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a8b6c276535474109cda8e1d0470884a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId1</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a98c214c9136d2f10c56c5afe19290b74</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId2</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6aed10335c1d89e1748b657336b10a8282</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId3</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a1212637a22b754b7a391ce9b21038ebe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId4</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6ab03087831525f839f19c44a9d79d20df</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId5</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a068da01ec533d85f1b2f098c7b4b2040</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>whiteColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a73a50ed25cfa50172870d472227fc137</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>blackColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6ac7411db48774329c05a27e414ee24595</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>displayColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6aa7af29d3637768fbf97690f5a7d59e5c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6aa4bd0ee587cfbfef51b3b25d622b8a48</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>groupColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6ab08bc4b9b520e583faed901e1e76e59e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lightGroupColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a2864996ef37b5bf663448c3f8ae6f6a7</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>grey30ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a8836ddb4187fd9683c6952817859a255</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>grey45ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a66a982d75840d72c1c9ef4eb0a302fd6</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>grey60ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a87882c3f6219ea41cd1e0522707f4aa5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>grey90ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a7b989dd098f7a0747c523071621e4766</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>glass1ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a36769c6185d02150c1c87ffa812cac37</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>glass2ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a2d85184728940d03f032a57e5b6915f3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>matte1ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a9574cb8d76e0b587045e42ba6f560e3e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>matte2ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a92300711403ae30883fac16299863490</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>title1ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a48e1027db85fa69981be45218bd7e6d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>title2ColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a539354805ecf1dd789adb31680e32326</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>accentColourId</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a8b6c276535474109cda8e1d0470884a0</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId1</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a98c214c9136d2f10c56c5afe19290b74</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId2</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6aed10335c1d89e1748b657336b10a8282</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId3</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a1212637a22b754b7a391ce9b21038ebe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId4</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6ab03087831525f839f19c44a9d79d20df</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>colourId5</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a3444b16d6592301f4a2179bb650885c6a068da01ec533d85f1b2f098c7b4b2040</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>GinLookAndFeel</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>ae2f7157e485bb30b42f4b47e617a3c17</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::Colour</type>
      <name>defaultColour</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a13e4ce006239d68f35c217261a9ed907</anchor>
      <arglist>(int idx)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawRotarySlider</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>abd86ed5df86e07320f192df0c1dc9288</anchor>
      <arglist>(juce::Graphics &amp;, int x, int y, int width, int height, float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawLinearSlider</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>ad3d6d856e5508baa4397c3b3d3b2aff3</anchor>
      <arglist>(juce::Graphics &amp;, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos, const juce::Slider::SliderStyle, juce::Slider &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawButtonBackground</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a73361bf8910f40eb4833ce02bb0a34de</anchor>
      <arglist>(juce::Graphics &amp;, juce::Button &amp;, const juce::Colour &amp;backgroundColour, bool isMouseOverButton, bool isButtonDown) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawButtonText</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>aaeb486d5d6926aaf12339fbcaee2dfd6</anchor>
      <arglist>(juce::Graphics &amp;, juce::TextButton &amp;, bool isMouseOverButton, bool isButtonDown) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawComboBox</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>ae3ae1eba4e69ec80c57454bb7e79f3d6</anchor>
      <arglist>(juce::Graphics &amp;, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>positionComboBoxText</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>af9f9187b11f3e290fab683fc65cfb914</anchor>
      <arglist>(juce::ComboBox &amp;, juce::Label &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>drawTextEditorOutline</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>aa6bf04df21fad6a9fd1b01cf0d5c7658</anchor>
      <arglist>(juce::Graphics &amp;, int width, int height, juce::TextEditor &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getAlertWindowButtonHeight</name>
      <anchorfile>classGinLookAndFeel.html</anchorfile>
      <anchor>a97c257dafafcaeb191d2f3dbd4bbf546</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>GlideInfo</name>
    <filename>structGlideInfo.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>fromNote</name>
      <anchorfile>structGlideInfo.html</anchorfile>
      <anchor>a7c71430c47f2873746b054f468ac0b15</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>glissando</name>
      <anchorfile>structGlideInfo.html</anchorfile>
      <anchor>a9d07e5c884046e64cf09d6b24a26b69e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>portamento</name>
      <anchorfile>structGlideInfo.html</anchorfile>
      <anchor>a1e24c72c427d5a6c428e263fd51bc774</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>rate</name>
      <anchorfile>structGlideInfo.html</anchorfile>
      <anchor>add32d1f350249b4c824f299cb58efa8e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>HeaderItem</name>
    <filename>classHeaderItem.html</filename>
    <member kind="function">
      <type></type>
      <name>HeaderItem</name>
      <anchorfile>classHeaderItem.html</anchorfile>
      <anchor>ac6986963c5c20c0df2a1d82795579440</anchor>
      <arglist>(juce::String name_, Parameter *enable, ModMatrix &amp;mm, ModSrcId monoSrc={}, ModSrcId polySrc={})</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>HeaderItem</name>
      <anchorfile>classHeaderItem.html</anchorfile>
      <anchor>a1d107c83356c617db19288db61f6aedd</anchor>
      <arglist>(juce::String name_, Parameter *enable=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSelected</name>
      <anchorfile>classHeaderItem.html</anchorfile>
      <anchor>afc6b0457f1d53e28e7381be7f70b083d</anchor>
      <arglist>(bool s)</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void()&gt;</type>
      <name>onClick</name>
      <anchorfile>classHeaderItem.html</anchorfile>
      <anchor>ab84fe8f66c5a6f45a800053b4f11ab30</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>HeaderRow</name>
    <filename>classHeaderRow.html</filename>
    <member kind="function">
      <type></type>
      <name>HeaderRow</name>
      <anchorfile>classHeaderRow.html</anchorfile>
      <anchor>a964daf8517b543beb3e4592436f3138c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addItem</name>
      <anchorfile>classHeaderRow.html</anchorfile>
      <anchor>a770bf3f4fe897fc526d5e3268748f404</anchor>
      <arglist>(HeaderItem &amp;item)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>HorizontalFader</name>
    <filename>classHorizontalFader.html</filename>
    <base>ParamComponent</base>
    <member kind="function">
      <type></type>
      <name>HorizontalFader</name>
      <anchorfile>classHorizontalFader.html</anchorfile>
      <anchor>ad34bdd3d394ea50181cfb8a8e32a55bd</anchor>
      <arglist>(Parameter *parameter, bool fromCentre=false)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>HP12State</name>
    <filename>structHP12State.html</filename>
    <base>FuncState</base>
    <member kind="function">
      <type></type>
      <name>HP12State</name>
      <anchorfile>structHP12State.html</anchorfile>
      <anchor>a26cb6edccf0ef60fcd94b972c61a0491</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>process</name>
      <anchorfile>structHP12State.html</anchorfile>
      <anchor>a8cfe063b5f1b95fcfbaca7e1762d7ed8</anchor>
      <arglist>(double v, double freq, double q)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structHP12State.html</anchorfile>
      <anchor>ac3a70a8cdf429158cbd0f6f326a86a6a</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter</name>
      <anchorfile>structHP12State.html</anchorfile>
      <anchor>a006d9aa1837a5c2b25d70cbd303c021b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>HP24State</name>
    <filename>structHP24State.html</filename>
    <base>FuncState</base>
    <member kind="function">
      <type></type>
      <name>HP24State</name>
      <anchorfile>structHP24State.html</anchorfile>
      <anchor>a893b30c4df910670f54f57fdb8adc80d</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>process</name>
      <anchorfile>structHP24State.html</anchorfile>
      <anchor>ab27a6d9f554faf25e83ac000052f0f2e</anchor>
      <arglist>(double v, double freq, double q)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structHP24State.html</anchorfile>
      <anchor>a9f296d72bbd2c0781131fd21c1e1a6d4</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter1</name>
      <anchorfile>structHP24State.html</anchorfile>
      <anchor>a155f66fcce4f834299723b0e17f45a08</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter2</name>
      <anchorfile>structHP24State.html</anchorfile>
      <anchor>a50cf6b5b1abc8cde792a5ef60de47bd3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Http</name>
    <filename>classHttp.html</filename>
    <class kind="struct">Http::HttpResult</class>
    <member kind="function">
      <type></type>
      <name>Http</name>
      <anchorfile>classHttp.html</anchorfile>
      <anchor>ad057da75ec75c9a44f3b736ff19ccd9d</anchor>
      <arglist>(juce::URL url)</arglist>
    </member>
    <member kind="function">
      <type>HttpResult</type>
      <name>get</name>
      <anchorfile>classHttp.html</anchorfile>
      <anchor>aa25b6e9f58fe6924809e801bc8eefe21</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>Http::HttpResult</name>
    <filename>structHttp_1_1HttpResult.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>statusCode</name>
      <anchorfile>structHttp_1_1HttpResult.html</anchorfile>
      <anchor>a503d39d8b2bb60b51594b5108038aca6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::StringPairArray</type>
      <name>headers</name>
      <anchorfile>structHttp_1_1HttpResult.html</anchorfile>
      <anchor>a663dee713583fca5be89fab8eddd3293</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::MemoryBlock</type>
      <name>data</name>
      <anchorfile>structHttp_1_1HttpResult.html</anchorfile>
      <anchor>a3ae0b311189f4413f1128ba994c2aca3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ImageMetadata</name>
    <filename>classImageMetadata.html</filename>
    <member kind="function">
      <type></type>
      <name>ImageMetadata</name>
      <anchorfile>classImageMetadata.html</anchorfile>
      <anchor>a1dfe334219bb1df21a42d95653dd326c</anchor>
      <arglist>(const juce::String &amp;type)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~ImageMetadata</name>
      <anchorfile>classImageMetadata.html</anchorfile>
      <anchor>ada1ae652af0b4e1341dd75376c2b8da3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getType</name>
      <anchorfile>classImageMetadata.html</anchorfile>
      <anchor>ab2d57b041126841465ef5e702000015b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual juce::StringPairArray</type>
      <name>getAllMetadata</name>
      <anchorfile>classImageMetadata.html</anchorfile>
      <anchor>a14426b0a404e962422140565f7a30217</anchor>
      <arglist>() const =0</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>getFromImage</name>
      <anchorfile>classImageMetadata.html</anchorfile>
      <anchor>a2370391c426e1544b439a6b556f0109c</anchor>
      <arglist>(juce::InputStream &amp;is, juce::OwnedArray&lt; ImageMetadata &gt; &amp;metadata)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::String</type>
      <name>type</name>
      <anchorfile>classImageMetadata.html</anchorfile>
      <anchor>a34bb47451a46175f4f65173cc8d6b3c8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Images</name>
    <filename>classImages.html</filename>
    <member kind="function">
      <type></type>
      <name>Images</name>
      <anchorfile>classImages.html</anchorfile>
      <anchor>a3c31a7052197172e2bfb26efe796d32f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>juce::Path</type>
      <name>monoPath</name>
      <anchorfile>classImages.html</anchorfile>
      <anchor>a1d271cdd9f31586a82eab975793289af</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::Path</type>
      <name>polyPath</name>
      <anchorfile>classImages.html</anchorfile>
      <anchor>a256e9bfb3e202edc5d3a1496b139a259</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::Path</type>
      <name>powerPath</name>
      <anchorfile>classImages.html</anchorfile>
      <anchor>acd6375f27acc618531676b5529792e28</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SingleLineTextEditor::InputFilter</name>
    <filename>classSingleLineTextEditor_1_1InputFilter.html</filename>
    <member kind="function">
      <type></type>
      <name>InputFilter</name>
      <anchorfile>classSingleLineTextEditor_1_1InputFilter.html</anchorfile>
      <anchor>a99b6d8607dd45a320cddd4b3bd18133b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~InputFilter</name>
      <anchorfile>classSingleLineTextEditor_1_1InputFilter.html</anchorfile>
      <anchor>aa8468b2a52e2f9cbd642f407c77bb419</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual juce::String</type>
      <name>filterNewText</name>
      <anchorfile>classSingleLineTextEditor_1_1InputFilter.html</anchorfile>
      <anchor>a9f92ce46cd2c3aa2eaf1a5b0734af0df</anchor>
      <arglist>(SingleLineTextEditor &amp;, const juce::String &amp;newInput)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Integrator</name>
    <filename>classIntegrator.html</filename>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classIntegrator.html</anchorfile>
      <anchor>aa0d9e23b5fb6adcd8f10d51fb4543bbf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getIntegral</name>
      <anchorfile>classIntegrator.html</anchorfile>
      <anchor>a20e85f1f0f94fac676fcd91828223646</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPoint</name>
      <anchorfile>classIntegrator.html</anchorfile>
      <anchor>a9505d0e4528b35956870924bce7a8849</anchor>
      <arglist>(double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPoint</name>
      <anchorfile>classIntegrator.html</anchorfile>
      <anchor>a7e83337e1ae73f42037550ca2d5106f8</anchor>
      <arglist>(Point&lt; double &gt; point)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPoints</name>
      <anchorfile>classIntegrator.html</anchorfile>
      <anchor>af14485019c60ecac13672284b47d8adf</anchor>
      <arglist>(juce::Array&lt; Point&lt; double &gt;&gt; points)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>IptcMetadata</name>
    <filename>classIptcMetadata.html</filename>
    <base>ImageMetadata</base>
    <member kind="function">
      <type></type>
      <name>IptcMetadata</name>
      <anchorfile>classIptcMetadata.html</anchorfile>
      <anchor>a496280ef9815903894104ab237831523</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~IptcMetadata</name>
      <anchorfile>classIptcMetadata.html</anchorfile>
      <anchor>a59529ad6881125efe51aeb06ffe47d6b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::StringPairArray</type>
      <name>getAllMetadata</name>
      <anchorfile>classIptcMetadata.html</anchorfile>
      <anchor>a43e8dc2bcf8326475805048a5dfe9c87</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static IptcMetadata *</type>
      <name>create</name>
      <anchorfile>classIptcMetadata.html</anchorfile>
      <anchor>a93ea3817dd0fe4476c8eb3f3a17e201a</anchor>
      <arglist>(const juce::uint8 *data, int sz)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Knob</name>
    <filename>classKnob.html</filename>
    <base>ParamComponent</base>
    <base protection="private">ModMatrix::Listener</base>
    <member kind="function">
      <type></type>
      <name>Knob</name>
      <anchorfile>classKnob.html</anchorfile>
      <anchor>a6988e2f33ec94e29f818eb0126f351bb</anchor>
      <arglist>(Parameter *parameter, bool fromCentre=false)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Knob</name>
      <anchorfile>classKnob.html</anchorfile>
      <anchor>a8b061711ecd0109e6244e3ce8b99f8c4</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLiveValuesCallback</name>
      <anchorfile>classKnob.html</anchorfile>
      <anchor>aeb0d9897db4e09830a30994ba7cfc161</anchor>
      <arglist>(std::function&lt; juce::Array&lt; float &gt;()&gt; cb)</arglist>
    </member>
    <member kind="function">
      <type>PluginSlider &amp;</type>
      <name>getSlider</name>
      <anchorfile>classKnob.html</anchorfile>
      <anchor>a5cc64c2f000e2044327dd10d031f4925</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LambdaAsyncUpdater</name>
    <filename>classLambdaAsyncUpdater.html</filename>
    <member kind="function">
      <type></type>
      <name>LambdaAsyncUpdater</name>
      <anchorfile>classLambdaAsyncUpdater.html</anchorfile>
      <anchor>ad20796089a208033f56f6146ab623f34</anchor>
      <arglist>(std::function&lt; void()&gt; func)</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void()&gt;</type>
      <name>onAsyncUpdate</name>
      <anchorfile>classLambdaAsyncUpdater.html</anchorfile>
      <anchor>a74237db5842e8c791b51fbe19f22cd6c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LambdaTimer</name>
    <filename>classLambdaTimer.html</filename>
    <member kind="function">
      <type></type>
      <name>LambdaTimer</name>
      <anchorfile>classLambdaTimer.html</anchorfile>
      <anchor>a4156bda8cd4d7bc5f288138900a1c068</anchor>
      <arglist>(std::function&lt; void()&gt; func=nullptr)</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void()&gt;</type>
      <name>onTimer</name>
      <anchorfile>classLambdaTimer.html</anchorfile>
      <anchor>a36a3d43e53e8d19d84262b31f7869977</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LambdaValueTreeListener</name>
    <filename>classLambdaValueTreeListener.html</filename>
    <member kind="function">
      <type></type>
      <name>LambdaValueTreeListener</name>
      <anchorfile>classLambdaValueTreeListener.html</anchorfile>
      <anchor>a3f7db4c387824d9d43e7de3ce17898ec</anchor>
      <arglist>(juce::ValueTree &amp;v_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~LambdaValueTreeListener</name>
      <anchorfile>classLambdaValueTreeListener.html</anchorfile>
      <anchor>ac6413bb03bebc68e72e4992ce284430e</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(juce::ValueTree &amp;, const juce::Identifier &amp;)&gt;</type>
      <name>onValueTreePropertyChanged</name>
      <anchorfile>classLambdaValueTreeListener.html</anchorfile>
      <anchor>a84758404d4b1bb632946767d5efb6b2b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(juce::ValueTree &amp;, juce::ValueTree &amp;)&gt;</type>
      <name>onValueTreeChildAdded</name>
      <anchorfile>classLambdaValueTreeListener.html</anchorfile>
      <anchor>a8ae2510546ac5459bb59c883e699d7f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(juce::ValueTree &amp;, juce::ValueTree &amp;, int)&gt;</type>
      <name>onValueTreeChildRemoved</name>
      <anchorfile>classLambdaValueTreeListener.html</anchorfile>
      <anchor>a92dae3b8c8fcaa94a9b0286a8e4d67d8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(juce::ValueTree &amp;, int, int)&gt;</type>
      <name>onValueTreeChildOrderChanged</name>
      <anchorfile>classLambdaValueTreeListener.html</anchorfile>
      <anchor>ab24d4441320b9d5769d4240126ccdf10</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(juce::ValueTree &amp;)&gt;</type>
      <name>onValueTreeParentChanged</name>
      <anchorfile>classLambdaValueTreeListener.html</anchorfile>
      <anchor>a1f4864cfd07b42efe993f6fe7c3d3732</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(juce::ValueTree &amp;)&gt;</type>
      <name>onValueTreeRedirected</name>
      <anchorfile>classLambdaValueTreeListener.html</anchorfile>
      <anchor>a947e309862d3e35bc7e51c3c35c25651</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Layout</name>
    <filename>classLayout.html</filename>
    <member kind="function">
      <type></type>
      <name>Layout</name>
      <anchorfile>classLayout.html</anchorfile>
      <anchor>adf1be658274eef9d8ef0e4307b9f5dee</anchor>
      <arglist>(juce::Component &amp;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Layout</name>
      <anchorfile>classLayout.html</anchorfile>
      <anchor>a9f53a76fe9a45d0f6325b42eadf51fa9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLayout</name>
      <anchorfile>classLayout.html</anchorfile>
      <anchor>a8f95d75f7456f3daaf7b2e657273946b</anchor>
      <arglist>(const juce::String &amp;filename, const juce::File &amp;source)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LeastSquaresRegression</name>
    <filename>classLeastSquaresRegression.html</filename>
    <member kind="function">
      <type></type>
      <name>LeastSquaresRegression</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>a853a9218262dfc2f4d2d99dc014ef15b</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPoint</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>a1b58c3452c171d5c474021c6edb5a6c2</anchor>
      <arglist>(double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPoint</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>a672bba942baa8b2e81d8514b35942be8</anchor>
      <arglist>(Point&lt; double &gt; point)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPoints</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>ac4435a3ed89f43e2a0e59990ea6caa14</anchor>
      <arglist>(juce::Array&lt; Point&lt; double &gt;&gt; points)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>a07db19bc6cc01dfbb5a7575cf6c0a01e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>enoughPoints</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>a188573e2bd06e10a2c8ab3a3849e8df2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::Array&lt; double &gt;</type>
      <name>getTerms</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>ac369d5ec943d39d6d00b45cf381a989d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>aTerm</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>a5bfe5f2d56a0e4585102e95a752d17bf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>bTerm</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>af8a14f97edfe06e9237df6cbf5a54330</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cTerm</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>aea2509a184bc43dc3bfcf114b4a80da7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rSquare</name>
      <anchorfile>classLeastSquaresRegression.html</anchorfile>
      <anchor>a7314973ea0d9b54e0071adf48ebefbd0</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SingleLineTextEditor::LengthAndCharacterRestriction</name>
    <filename>classSingleLineTextEditor_1_1LengthAndCharacterRestriction.html</filename>
    <base>SingleLineTextEditor::InputFilter</base>
    <member kind="function">
      <type></type>
      <name>LengthAndCharacterRestriction</name>
      <anchorfile>classSingleLineTextEditor_1_1LengthAndCharacterRestriction.html</anchorfile>
      <anchor>a4d5a40f6acac1514cf9533a06f41197a</anchor>
      <arglist>(int maxNumChars, const juce::String &amp;allowedCharacters)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>filterNewText</name>
      <anchorfile>classSingleLineTextEditor_1_1LengthAndCharacterRestriction.html</anchorfile>
      <anchor>a267b86a047f037c1b84d32bf32c2b6d9</anchor>
      <arglist>(SingleLineTextEditor &amp;, const juce::String &amp;) override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LevelMeter</name>
    <filename>classLevelMeter.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ColourIds</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>aebb71a7df2ebf267fa0326a95142cfa3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>aebb71a7df2ebf267fa0326a95142cfa3a28b27caf96c83d8b60fa1e616e9bacaa</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>aebb71a7df2ebf267fa0326a95142cfa3aab214772ec0e99e6b061e5d8facc2893</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>meterColourId</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>aebb71a7df2ebf267fa0326a95142cfa3ab7530e0b591c2a089549783c99bb942e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>aebb71a7df2ebf267fa0326a95142cfa3a28b27caf96c83d8b60fa1e616e9bacaa</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>aebb71a7df2ebf267fa0326a95142cfa3aab214772ec0e99e6b061e5d8facc2893</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>meterColourId</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>aebb71a7df2ebf267fa0326a95142cfa3ab7530e0b591c2a089549783c99bb942e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LevelMeter</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>abc77032cf51d0f40dafa76337a7e329f</anchor>
      <arglist>(const LevelTracker &amp;, juce::NormalisableRange&lt; float &gt; r={-60, 0})</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~LevelMeter</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>a6b26974fbd72dfdc77990d656bf6a3a8</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTopDown</name>
      <anchorfile>classLevelMeter.html</anchorfile>
      <anchor>ad1d2a77286974a55c9a03b8d2de6b498</anchor>
      <arglist>(bool td)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LevelTracker</name>
    <filename>classLevelTracker.html</filename>
    <member kind="function">
      <type></type>
      <name>LevelTracker</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>a3397f9df726242a093084a5856c0f4c8</anchor>
      <arglist>(float decayPerSecond=30.0f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>trackBuffer</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>ae015d22dde9a4f1241fdcc062a58d1a8</anchor>
      <arglist>(const float *buffer, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>trackBuffer</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>a1fc8ce4c2c22346db754bb3f5a544bf2</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>trackSample</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>a94deea05c53ad4155870361ffc56b3be</anchor>
      <arglist>(float f)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getLevel</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>a92624a1a9465c40709d2feaf93795ba6</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getClip</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>a9c47381ecd5162c2d00d685356506f1c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearClip</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>a9c8f9b4b93b03157c3ba834a11d33620</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>peakTime</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>a88d4d7383672bfbf10826e954d9cf408</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>peakLevel</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>ae36cb9c622a3230e0a7f24a5c9b621ac</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>decayRate</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>a796f0f65de9c1017f59fa6dc0247ca83</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>clip</name>
      <anchorfile>classLevelTracker.html</anchorfile>
      <anchor>a5702532186fb2626960f2e4a00f17a38</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LFO</name>
    <filename>classLFO.html</filename>
    <class kind="struct">LFO::Parameters</class>
    <member kind="enumeration">
      <type></type>
      <name>WaveShape</name>
      <anchorfile>classLFO.html</anchorfile>
      <anchor>a9377c9fb47d163e9343a11edd200da29</anchor>
      <arglist></arglist>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a334c4a4c42fdb79d7ebc3e73b517e6f8">none</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a925212d1624625aed9491413b4b7be05">sine</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29acef44b46f16ae8ecf664df4266ffdbf9">triangle</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29acf87f9eb2bfacc5a0362031eba539290">sawUp</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a3593df94728cd88f078bbc6c41df9876">sawDown</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a2fc01ec765ec0cb3dcc559126de20b30">square</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a8b70dd8f1d0ecc516d51a67b21488158">squarePos</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29ad674bb86b600f7194b224cfdddb926be">sampleAndHold</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29aaaddc3454ccbefbb2d8d8461f8f7f481">noise</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a47a947491015719eaaa2e001e86eea54">stepUp3</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29af71d95a4c6af33601854ca4ebe2a5adc">stepUp4</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a528e39dd5263c6b717cdab4896b12f43">stepup8</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29aaa3d13ace9a6e4045e504d765616f1c8">stepDown3</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29aea3c4dcae408cbe4ca833cc5deab05a8">stepDown4</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a60e4b0ffa818d9dfcae5f4f8e163c7de">stepDown8</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29ad5e3304893d6f84f5210fe5ccd2f7888">pyramid3</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a3d7573264eadab6786ec51508c56ca27">pyramid5</enumvalue>
      <enumvalue file="classLFO.html" anchor="a9377c9fb47d163e9343a11edd200da29a17429d7baab06afa5e9bb17e8d474c51">pyramid9</enumvalue>
    </member>
    <member kind="function">
      <type></type>
      <name>LFO</name>
      <anchorfile>classLFO.html</anchorfile>
      <anchor>ad89e6308102d5b8f0790b2c8cc89c4a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classLFO.html</anchorfile>
      <anchor>ac53ad3892743661db85583bcf3b4cbf2</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParameters</name>
      <anchorfile>classLFO.html</anchorfile>
      <anchor>ab516f44f799e5a8a3c784621139003b7</anchor>
      <arglist>(Parameters p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classLFO.html</anchorfile>
      <anchor>a643d01959a7f5b014335c96ff5af0fd6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classLFO.html</anchorfile>
      <anchor>ab673487f71e6a393c9ef1091a0bfdf82</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>process</name>
      <anchorfile>classLFO.html</anchorfile>
      <anchor>aecd209c8d601e2b23aa9eaee2bb0f075</anchor>
      <arglist>(int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getOutput</name>
      <anchorfile>classLFO.html</anchorfile>
      <anchor>a87dce294e659cd21de22aaa2f7de6119</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LFOComponent</name>
    <filename>classLFOComponent.html</filename>
    <base>MultiParamComponent</base>
    <member kind="function">
      <type></type>
      <name>LFOComponent</name>
      <anchorfile>classLFOComponent.html</anchorfile>
      <anchor>a15b40bec5169584567c4f58c3e1e7687</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~LFOComponent</name>
      <anchorfile>classLFOComponent.html</anchorfile>
      <anchor>a3c71a2b379552967cb829cec1a244489</anchor>
      <arglist>() override=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setBPM</name>
      <anchorfile>classLFOComponent.html</anchorfile>
      <anchor>a7afaee1843122ca663f8b371575e6030</anchor>
      <arglist>(float bpm)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classLFOComponent.html</anchorfile>
      <anchor>a4a9ee52cbf8567eb288fe1cfdd1cd990</anchor>
      <arglist>(Parameter::Ptr wave, Parameter::Ptr sync, Parameter::Ptr rate, Parameter::Ptr beat, Parameter::Ptr depth, Parameter::Ptr offset, Parameter::Ptr phase, Parameter::Ptr enable)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LinearRegression</name>
    <filename>classLinearRegression.html</filename>
    <member kind="function">
      <type></type>
      <name>LinearRegression</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>afeb4c101e4e8b20a75db3d4ce3f1cbc6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRegression</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a940aeba49f6ecfd5da77c4a1bc2a532c</anchor>
      <arglist>(juce::Array&lt; Point&lt; double &gt;&gt; points)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPoint</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>aa263ad3c4a2577b1065cac7a356f9e76</anchor>
      <arglist>(Point&lt; double &gt; pnt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>calculate</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a06a87f9496539d592d355e55f1f0f1c5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a5d9abca564aa9f8113bfe99b4ffe2307</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>haveData</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>ab0d8b0bd896a4ac1a6d0fad86d5de10b</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>items</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>ac40577bbf7fc49ffb578e0ed42f2c5f6</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getA</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a89e9199f1e6e28c47151c7dc97c358da</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getB</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>ac1ca4812272ecf0dca2cc80854adddb4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getCoefDeterm</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>af563cd8f237f0d5c7a0b5319f3a90296</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getCoefCorrel</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a86d1866295db2e234f21ddf780062609</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getStdErrorEst</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a2e64de9960a1d7d395a8cbb7ee91b860</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>estimateY</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a1321792d996e1d6b2b7c4b15e571d7d5</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>n</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a2538a18e56f7da311ae94edbb903e1bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sumX</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a2d137eb916f6e2f3cf552b3f1a12a4d0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sumY</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>ae62507e4f1f97b8ab2faae5112ce97d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sumXsquared</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a2b06ee3c27a6bc1e6fc709c0224dd949</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sumYsquared</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>aefdf526f61f6f62a24472ef3ce135d47</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>sumXY</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>af2744e010ca51a40db51b7e30b769390</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>a</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a9afb5e0ec8d6cbb6ac46e5915afc57ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>b</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>aa258a2c864179aaff99098dea9abf1cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>coefD</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a0f3377c655ca712c3e2c3d4e2125441c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>coefC</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>afdc67d71c93cea69b0a66693fa5cb276</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>stdError</name>
      <anchorfile>classLinearRegression.html</anchorfile>
      <anchor>a0cdd5962a78b99b95c6a2bc7c9fbb6a6</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>FileSystemWatcher::Listener</name>
    <filename>classFileSystemWatcher_1_1Listener.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Listener</name>
      <anchorfile>classFileSystemWatcher_1_1Listener.html</anchorfile>
      <anchor>a2f2bd934de14814b9cd28b4ea08278ad</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>folderChanged</name>
      <anchorfile>classFileSystemWatcher_1_1Listener.html</anchorfile>
      <anchor>a04a461ad2d135495403372fcc9d413dd</anchor>
      <arglist>(const juce::File)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>fileChanged</name>
      <anchorfile>classFileSystemWatcher_1_1Listener.html</anchorfile>
      <anchor>a74adb289b69441dd79b84f5eb6a66d59</anchor>
      <arglist>(const juce::File, FileSystemEvent)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ModMatrix::Listener</name>
    <filename>classModMatrix_1_1Listener.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Listener</name>
      <anchorfile>classModMatrix_1_1Listener.html</anchorfile>
      <anchor>a09cc42d56d116a600586a874bbe348a7</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>modMatrixChanged</name>
      <anchorfile>classModMatrix_1_1Listener.html</anchorfile>
      <anchor>a6d4c1fcaddd8a3f77a0b25813919eae3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>learnSourceChanged</name>
      <anchorfile>classModMatrix_1_1Listener.html</anchorfile>
      <anchor>a98f0bf4eb29994c864db47a9461dbc24</anchor>
      <arglist>(ModSrcId)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>OpenStreetMaps::Listener</name>
    <filename>classOpenStreetMaps_1_1Listener.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Listener</name>
      <anchorfile>classOpenStreetMaps_1_1Listener.html</anchorfile>
      <anchor>aeb7faf41923db2a12f09179159d96bd1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>tileFetched</name>
      <anchorfile>classOpenStreetMaps_1_1Listener.html</anchorfile>
      <anchor>a5a86659b291d99adbe0af1a30fdfe383</anchor>
      <arglist>(int zoom, int x, int y)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SingleLineTextEditor::Listener</name>
    <filename>classSingleLineTextEditor_1_1Listener.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Listener</name>
      <anchorfile>classSingleLineTextEditor_1_1Listener.html</anchorfile>
      <anchor>af52267b829ff17fd4e36d3a8dc8d99ff</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>sltextEditorTextChanged</name>
      <anchorfile>classSingleLineTextEditor_1_1Listener.html</anchorfile>
      <anchor>a7991331aa8048236d763b2482461a5a6</anchor>
      <arglist>(SingleLineTextEditor &amp;)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>sltextEditorReturnKeyPressed</name>
      <anchorfile>classSingleLineTextEditor_1_1Listener.html</anchorfile>
      <anchor>a2a4144e3e91e38142f9498eb66c180e6</anchor>
      <arglist>(SingleLineTextEditor &amp;)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>sltextEditorEscapeKeyPressed</name>
      <anchorfile>classSingleLineTextEditor_1_1Listener.html</anchorfile>
      <anchor>ae23dbf95e2aada75cb3f513d393b965d</anchor>
      <arglist>(SingleLineTextEditor &amp;)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>sltextEditorFocusLost</name>
      <anchorfile>classSingleLineTextEditor_1_1Listener.html</anchorfile>
      <anchor>a9b377e28f1ec65c2a9864f1c08964af3</anchor>
      <arglist>(SingleLineTextEditor &amp;)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>SingleLineTextEditor::LookAndFeelMethods</name>
    <filename>structSingleLineTextEditor_1_1LookAndFeelMethods.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~LookAndFeelMethods</name>
      <anchorfile>structSingleLineTextEditor_1_1LookAndFeelMethods.html</anchorfile>
      <anchor>a626aa634176d5f5fbb6e0c0b8b81221a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>fillSingleLineTextEditorBackground</name>
      <anchorfile>structSingleLineTextEditor_1_1LookAndFeelMethods.html</anchorfile>
      <anchor>a1e2b0252dace06516a350c7f3680f7c3</anchor>
      <arglist>(juce::Graphics &amp;, int width, int height, SingleLineTextEditor &amp;)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>drawSingleLineTextEditorOutline</name>
      <anchorfile>structSingleLineTextEditor_1_1LookAndFeelMethods.html</anchorfile>
      <anchor>a5db78989d26586b1239a954964b455bd</anchor>
      <arglist>(juce::Graphics &amp;, int width, int height, SingleLineTextEditor &amp;)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual juce::CaretComponent *</type>
      <name>createSingleLineCaretComponent</name>
      <anchorfile>structSingleLineTextEditor_1_1LookAndFeelMethods.html</anchorfile>
      <anchor>a90d47e49d33175a975e89eeed7ef7f89</anchor>
      <arglist>(Component *keyFocusOwner)=0</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LP12State</name>
    <filename>structLP12State.html</filename>
    <base>FuncState</base>
    <member kind="function">
      <type></type>
      <name>LP12State</name>
      <anchorfile>structLP12State.html</anchorfile>
      <anchor>a439292ecb27bfe33097d9ba0255c7a02</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>process</name>
      <anchorfile>structLP12State.html</anchorfile>
      <anchor>a6992aaecb455861b653624eae79e7040</anchor>
      <arglist>(double v, double freq, double q)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structLP12State.html</anchorfile>
      <anchor>a1c258db907dd09dd5803681d8186adf1</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter</name>
      <anchorfile>structLP12State.html</anchorfile>
      <anchor>a580c0bf2f2bbb272aec09a28fe98f23a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LP24State</name>
    <filename>structLP24State.html</filename>
    <base>FuncState</base>
    <member kind="function">
      <type></type>
      <name>LP24State</name>
      <anchorfile>structLP24State.html</anchorfile>
      <anchor>aa49c9ae5be422d7eb09fd3b37b552fec</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>process</name>
      <anchorfile>structLP24State.html</anchorfile>
      <anchor>a00f9ddbb54b12dfb6d99b20d4b640779</anchor>
      <arglist>(double v, double freq, double q)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structLP24State.html</anchorfile>
      <anchor>a005e50da3e6b65df648b2be53d038016</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter1</name>
      <anchorfile>structLP24State.html</anchorfile>
      <anchor>a8aaae7ba5553a0bc1880ea6fe340f1b7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter2</name>
      <anchorfile>structLP24State.html</anchorfile>
      <anchor>abc4ac3ff59b983b6cfaae11af5cc584f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MapViewer</name>
    <filename>classMapViewer.html</filename>
    <base protection="private">OpenStreetMaps::Listener</base>
    <member kind="function">
      <type></type>
      <name>MapViewer</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a6a5384d59042be977032bdabb02adcdd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~MapViewer</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a2016972d6feb583ac3adcdc41a1978d0</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZoom</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>ac55420a2219e2a30c03062411e0b2df6</anchor>
      <arglist>(int zoom)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>centerOn</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a77a364ca834883bbc75428ba81e92f79</anchor>
      <arglist>(double longCenter, double latCenter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>centerUnderPt</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a78d97113c9fff03aa70d4f334bf0a125</anchor>
      <arglist>(juce::Point&lt; double &gt; world, juce::Point&lt; int &gt; view)</arglist>
    </member>
    <member kind="function">
      <type>OpenStreetMaps *</type>
      <name>getOpenStreetMaps</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a376101640a1f33a7cbcc647eb93faf85</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>resized</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>ad62d5376f219c6df764f23e6d1352e8f</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a3a5a499a937627d3cdb7cd8417f02732</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mouseDown</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a41c743eb11ab2da7d670ec3a25e5d39e</anchor>
      <arglist>(const juce::MouseEvent &amp;e) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mouseDrag</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>af2a70d5d74faafa6faf3b8834560e64b</anchor>
      <arglist>(const juce::MouseEvent &amp;e) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mouseWheelMove</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a4bf0f24c8681d975a5f424c15c599c2d</anchor>
      <arglist>(const juce::MouseEvent &amp;e, const juce::MouseWheelDetails &amp;wheel) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>tileFetched</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>af0bcb61ce440c285e3cb8ff4f73494d2</anchor>
      <arglist>(int zoom, int x, int y) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>updateMap</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a723ad37abb7af9ecac0bdd68dd628e46</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>mapUpdated</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a92969f79ce00707cf7e9b13e004b8bed</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>preferencesChanged</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>a3b1865a61ae82def296883bdc0d34083</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>saveSnapshot</name>
      <anchorfile>classMapViewer.html</anchorfile>
      <anchor>aac4c02e7984a2688c550e7aa20764c40</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MessagePack</name>
    <filename>classMessagePack.html</filename>
    <member kind="function" static="yes">
      <type>static juce::MemoryBlock</type>
      <name>toMessagePack</name>
      <anchorfile>classMessagePack.html</anchorfile>
      <anchor>aee9464edf2e67d30bc42114eabf7e18c</anchor>
      <arglist>(const juce::var &amp;obj)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static juce::var</type>
      <name>parse</name>
      <anchorfile>classMessagePack.html</anchorfile>
      <anchor>af05e84a64218328ce6d733fcd80a960a</anchor>
      <arglist>(const juce::MemoryBlock &amp;data)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MidiFifo</name>
    <filename>classMidiFifo.html</filename>
    <member kind="function">
      <type></type>
      <name>MidiFifo</name>
      <anchorfile>classMidiFifo.html</anchorfile>
      <anchor>a93cad383f99aaaf001b0f9cc72903f19</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classMidiFifo.html</anchorfile>
      <anchor>a331f9791daac503178a4d4ba41766837</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getSamplesUsed</name>
      <anchorfile>classMidiFifo.html</anchorfile>
      <anchor>a676220e418ef97d59568709ace608f99</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>write</name>
      <anchorfile>classMidiFifo.html</anchorfile>
      <anchor>a90ece40a2c61b1bba7de907838fc7cf6</anchor>
      <arglist>(const juce::MidiBuffer &amp;midiSrc, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read</name>
      <anchorfile>classMidiFifo.html</anchorfile>
      <anchor>aa5502294f13bc7c5440cda2f0e7b2abf</anchor>
      <arglist>(juce::MidiBuffer &amp;midiDst, int numSamples)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>ModDstId</name>
    <filename>structModDstId.html</filename>
    <member kind="function">
      <type></type>
      <name>ModDstId</name>
      <anchorfile>structModDstId.html</anchorfile>
      <anchor>a5c07798cf7ac3ad89cf18d8956bf78d3</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ModDstId</name>
      <anchorfile>structModDstId.html</anchorfile>
      <anchor>a1cd816f0e6701f16922f19f3b5e64ccb</anchor>
      <arglist>(int id_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ModDstId</name>
      <anchorfile>structModDstId.html</anchorfile>
      <anchor>aa3f249a7a06342976e5740e09d281a9e</anchor>
      <arglist>(const ModDstId &amp;other)</arglist>
    </member>
    <member kind="function">
      <type>ModDstId &amp;</type>
      <name>operator=</name>
      <anchorfile>structModDstId.html</anchorfile>
      <anchor>ade6e4c1af3c4a41f6e0575e2c0846bd0</anchor>
      <arglist>(const ModDstId &amp;other)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>structModDstId.html</anchorfile>
      <anchor>a63b8ebeaadf0a9c266a36d1a4c0e3e84</anchor>
      <arglist>(const ModDstId &amp;other) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isValid</name>
      <anchorfile>structModDstId.html</anchorfile>
      <anchor>ac2faf98a1a45cadf932f74a8f93ed17c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>id</name>
      <anchorfile>structModDstId.html</anchorfile>
      <anchor>a113d55c0da8df5a57180514adc99492c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ModMatrix</name>
    <filename>classModMatrix.html</filename>
    <class kind="class">ModMatrix::Listener</class>
    <member kind="function">
      <type></type>
      <name>ModMatrix</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a923354065c39928704876d0b29e568c9</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stateUpdated</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>aa3b18298e7798ae8293f8beb6622db23</anchor>
      <arglist>(const juce::ValueTree &amp;vt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateState</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a9d2727c3ffce6935f804eff00495e4f8</anchor>
      <arglist>(juce::ValueTree &amp;vt)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getValue</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>adc443dda62a48732e2bf333e34b8b128</anchor>
      <arglist>(gin::Parameter *p)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getValue</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a771bcda462681040c59c37a9e7ae6332</anchor>
      <arglist>(ModVoice &amp;voice, gin::Parameter *p)</arglist>
    </member>
    <member kind="function">
      <type>juce::Array&lt; float &gt;</type>
      <name>getLiveValues</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a313589d0966e9824aa05ae818e718280</anchor>
      <arglist>(gin::Parameter *p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMonoValue</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a059b88c04a7d524b96aa20034a943392</anchor>
      <arglist>(ModSrcId id, float value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPolyValue</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a223b6092cd0cae14bfd42e20935b3396</anchor>
      <arglist>(ModVoice &amp;voice, ModSrcId id, float value)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finishBlock</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>ad7ed486680d770f286653f045420db5a</anchor>
      <arglist>(int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addVoice</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>ac2d1103929b9eaf3659b9f9f59603975</anchor>
      <arglist>(ModVoice *v)</arglist>
    </member>
    <member kind="function">
      <type>ModSrcId</type>
      <name>addMonoModSource</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>ad3fc6378eb99507135d11c01944575cd</anchor>
      <arglist>(const juce::String &amp;id, const juce::String &amp;name, bool bipolar)</arglist>
    </member>
    <member kind="function">
      <type>ModSrcId</type>
      <name>addPolyModSource</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a66caa6c7b30ee5a492bde8c0d56c2520</anchor>
      <arglist>(const juce::String &amp;id, const juce::String &amp;name, bool bipolar)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addParameter</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a1a1235070a1660c1490ba5c2b9ab68f5</anchor>
      <arglist>(gin::Parameter *p, bool poly)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>aa5a44973523be3e13090e46bd3751777</anchor>
      <arglist>(double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>build</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a34daa946cb85440762ff041e995e670c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>enableLearn</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a7503eb612eccc0a07bafd3ba5c0c66b8</anchor>
      <arglist>(ModSrcId source)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>disableLearn</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a43c4fb3d9ece3cd9b2af6495379dc2f6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ModSrcId</type>
      <name>getLearn</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a97578cacf24fc1ca6145f7692338812f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumModSources</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a773b5e3fa24cf0c1834c9202ebb28eb9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getModSrcName</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a515297946b470b05c45f22dce83b6506</anchor>
      <arglist>(ModSrcId src)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getModSrcPoly</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>af53ec21a1331b38eeba0156effebf7dc</anchor>
      <arglist>(ModSrcId src)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getModSrcBipolar</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>ad2b193fcf8ef2790b41bc3bb2f4b0bf5</anchor>
      <arglist>(ModSrcId src)</arglist>
    </member>
    <member kind="function">
      <type>juce::Array&lt; ModSrcId &gt;</type>
      <name>getModSources</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a17ebf21dccd38b5d56d9d8d6357ae0c2</anchor>
      <arglist>(gin::Parameter *)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isModulated</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a85b92c5372a70175db5bc73486b371a1</anchor>
      <arglist>(ModDstId param)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getModDepth</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>ac09e09e0d0db90228b5b71013472efcb</anchor>
      <arglist>(ModSrcId src, ModDstId param)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setModDepth</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>ace5b6d8daf94ac3a9f13d61a322c2008</anchor>
      <arglist>(ModSrcId src, ModDstId param, float f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearModDepth</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>ab974f4fdc82957f79b98ae8f151baec7</anchor>
      <arglist>(ModSrcId src, ModDstId param)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addListener</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>a0b42fc976f683b259c00bdb2e636c6f2</anchor>
      <arglist>(Listener *l)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeListener</name>
      <anchorfile>classModMatrix.html</anchorfile>
      <anchor>adeaa67481f092f988ea3df3936909940</anchor>
      <arglist>(Listener *l)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ModMatrixBox</name>
    <filename>classModMatrixBox.html</filename>
    <base protection="private">ModMatrix::Listener</base>
    <member kind="function">
      <type></type>
      <name>ModMatrixBox</name>
      <anchorfile>classModMatrixBox.html</anchorfile>
      <anchor>acf1c7954f577749a28bf846cf9aafb4e</anchor>
      <arglist>(gin::Processor &amp;p, ModMatrix &amp;m)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ModMatrixBox</name>
      <anchorfile>classModMatrixBox.html</anchorfile>
      <anchor>a7513883f426c4df209ed3c5b78b99bd7</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>ModSrcId</name>
    <filename>structModSrcId.html</filename>
    <member kind="function">
      <type></type>
      <name>ModSrcId</name>
      <anchorfile>structModSrcId.html</anchorfile>
      <anchor>afd814dca3ee595f427b4e869f249f594</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ModSrcId</name>
      <anchorfile>structModSrcId.html</anchorfile>
      <anchor>a4648ac76f10dd3a15792224ef7727526</anchor>
      <arglist>(int id_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ModSrcId</name>
      <anchorfile>structModSrcId.html</anchorfile>
      <anchor>a9998764b350a96cb887a65424ec17d72</anchor>
      <arglist>(const ModSrcId &amp;other)</arglist>
    </member>
    <member kind="function">
      <type>ModSrcId &amp;</type>
      <name>operator=</name>
      <anchorfile>structModSrcId.html</anchorfile>
      <anchor>a1d9f52fd952b26811fc7e74a690f2c8e</anchor>
      <arglist>(const ModSrcId &amp;other)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>structModSrcId.html</anchorfile>
      <anchor>ae9a0aa774f9639b59f382127602af3eb</anchor>
      <arglist>(const ModSrcId &amp;other) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isValid</name>
      <anchorfile>structModSrcId.html</anchorfile>
      <anchor>ae18d81c2a1d9105a775a882648b24376</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>id</name>
      <anchorfile>structModSrcId.html</anchorfile>
      <anchor>ae8410470ac2d16b3d27179b6633efc65</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ModSrcListBox</name>
    <filename>classModSrcListBox.html</filename>
    <member kind="function">
      <type></type>
      <name>ModSrcListBox</name>
      <anchorfile>classModSrcListBox.html</anchorfile>
      <anchor>a5c4217751fb20f7a41b4734d179d85b3</anchor>
      <arglist>(ModMatrix &amp;m)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumRows</name>
      <anchorfile>classModSrcListBox.html</anchorfile>
      <anchor>a88055a83ab5d13fac08c28d2348f2aae</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paintListBoxItem</name>
      <anchorfile>classModSrcListBox.html</anchorfile>
      <anchor>af84a66b3d2128b5c42b04a976e022adf</anchor>
      <arglist>(int, juce::Graphics &amp;, int, int, bool) override</arglist>
    </member>
    <member kind="function">
      <type>Component *</type>
      <name>refreshComponentForRow</name>
      <anchorfile>classModSrcListBox.html</anchorfile>
      <anchor>a1b4ac4d3539934fc2249a5f1065fe417</anchor>
      <arglist>(int row, bool, Component *c) override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Modulation</name>
    <filename>classModulation.html</filename>
    <member kind="function">
      <type></type>
      <name>Modulation</name>
      <anchorfile>classModulation.html</anchorfile>
      <anchor>ad69ae957b7a70669951f70d3372ffb46</anchor>
      <arglist>(float maxDelay_=30.0f/1000.0f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classModulation.html</anchorfile>
      <anchor>a14bd355d13a545d034f30df8024102fe</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classModulation.html</anchorfile>
      <anchor>a7fa5c8e397f587f844315bb5fa192790</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classModulation.html</anchorfile>
      <anchor>a6c9e3b46d2c95cd13b61d61282f58e48</anchor>
      <arglist>(float delay_, float rate_, float depth_, float width_, float mix_)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classModulation.html</anchorfile>
      <anchor>a03d5d4643831d6249d5caa051279d5d2</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ModulationDestinationButton</name>
    <filename>classModulationDestinationButton.html</filename>
    <member kind="function">
      <type></type>
      <name>ModulationDestinationButton</name>
      <anchorfile>classModulationDestinationButton.html</anchorfile>
      <anchor>a57720d5c740821213b3390c85e181546</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ModulationDestinationButton</name>
      <anchorfile>classModulationDestinationButton.html</anchorfile>
      <anchor>a7962baa04f175c253ed31867e220a0fa</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ModulationOverview</name>
    <filename>classModulationOverview.html</filename>
    <base protection="private">ModMatrix::Listener</base>
    <member kind="function">
      <type></type>
      <name>ModulationOverview</name>
      <anchorfile>classModulationOverview.html</anchorfile>
      <anchor>a53905073cc95d9c56b70c931d0654a77</anchor>
      <arglist>(ModMatrix &amp;mm)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ModulationOverview</name>
      <anchorfile>classModulationOverview.html</anchorfile>
      <anchor>a0ee3b38ba40a5382c8f43589b44cfe59</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ModulationSourceButton</name>
    <filename>classModulationSourceButton.html</filename>
    <base protection="private">ModMatrix::Listener</base>
    <member kind="function">
      <type></type>
      <name>ModulationSourceButton</name>
      <anchorfile>classModulationSourceButton.html</anchorfile>
      <anchor>ae528e6ef08597a5535ec9b71b8478819</anchor>
      <arglist>(ModMatrix &amp;mm, ModSrcId src={}, bool poly_=false)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ModulationSourceButton</name>
      <anchorfile>classModulationSourceButton.html</anchorfile>
      <anchor>a44808e6b21763680da9c54bcefb6b362</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSource</name>
      <anchorfile>classModulationSourceButton.html</anchorfile>
      <anchor>a94555224817c529b26e4821706c97fcd</anchor>
      <arglist>(ModSrcId src, bool p)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ModVoice</name>
    <filename>classModVoice.html</filename>
    <member kind="function">
      <type></type>
      <name>ModVoice</name>
      <anchorfile>classModVoice.html</anchorfile>
      <anchor>ae3ba25475bbb4248e56a446575096da2</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~ModVoice</name>
      <anchorfile>classModVoice.html</anchorfile>
      <anchor>a4129fb9446d1da67a261115cd132ba76</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getValue</name>
      <anchorfile>group__gin__plugin-plugin.html</anchorfile>
      <anchor>gab084f6f52a2bf9f5141a63554b6d22bb</anchor>
      <arglist>(gin::Parameter *p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finishBlock</name>
      <anchorfile>classModVoice.html</anchorfile>
      <anchor>a3d415de2f815020b81365b8eb9951cd3</anchor>
      <arglist>(int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>snapParams</name>
      <anchorfile>classModVoice.html</anchorfile>
      <anchor>a000cad0edcefa2ca16fb616c7297d23c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>startVoice</name>
      <anchorfile>classModVoice.html</anchorfile>
      <anchor>aa987df0200c765566c0a359e3a2597b6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stopVoice</name>
      <anchorfile>classModVoice.html</anchorfile>
      <anchor>afc80f11e4f4853d40dd5f8665f19309a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getAge</name>
      <anchorfile>classModVoice.html</anchorfile>
      <anchor>a32fc0a65169eb9b73ebf39e97da8714a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual bool</type>
      <name>isVoiceActive</name>
      <anchorfile>classModVoice.html</anchorfile>
      <anchor>a031b2aad2c58ef9957553db94b61d0f2</anchor>
      <arglist>()=0</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>disableSmoothing</name>
      <anchorfile>classModVoice.html</anchorfile>
      <anchor>acc4675a7c50824248d15d4a416f5401b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>MultiParamComponent</name>
    <filename>classMultiParamComponent.html</filename>
    <base>Parameter::ParameterListener</base>
    <member kind="function">
      <type></type>
      <name>MultiParamComponent</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>a18f9b618b79b95f66337845843202a2e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~MultiParamComponent</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>a67ae20463b850068198ccc9427de6cc0</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>showBubble</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>a5cc5d97f95b48cac1c6d1989f4e1d971</anchor>
      <arglist>(const juce::String &amp;text, const juce::Rectangle&lt; int &gt; &amp;rc)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>hideBubble</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>a0f9a0c20b7c3dd25f063ca40dc421c20</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>watchParam</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>ac49a40f9a9abf19917dd3dc1b40bf5f8</anchor>
      <arglist>(Parameter *p)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>unwatchParams</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>ae7441578faba5891a7fc764da134f03c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>paramChanged</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>a55e25df161dbc7b98bcbd3a5f937561d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>enablementChanged</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>ae564f36608f33269bc246aad3df26478</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>valueUpdated</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>a5ea4449b0c06e5f5ed1acda230357b23</anchor>
      <arglist>(Parameter *) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>handleAsyncUpdate</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>af1f2a647d360783089b697dde0204e55</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::Array&lt; Parameter * &gt;</type>
      <name>params</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>a36bd41a594c81661474e30ee35c0eca5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; juce::BubbleMessageComponent &gt;</type>
      <name>bubble</name>
      <anchorfile>classMultiParamComponent.html</anchorfile>
      <anchor>afed6013095e1e2ced271cda5ccecb7d2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>NewsChecker</name>
    <filename>classNewsChecker.html</filename>
    <member kind="function">
      <type></type>
      <name>NewsChecker</name>
      <anchorfile>classNewsChecker.html</anchorfile>
      <anchor>a519004dfd3ae54255b2eff7a132e0a00</anchor>
      <arglist>(Processor &amp;slProc)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~NewsChecker</name>
      <anchorfile>classNewsChecker.html</anchorfile>
      <anchor>abc54683d73b189c02ccd30258fa99281</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getNewsUrl</name>
      <anchorfile>classNewsChecker.html</anchorfile>
      <anchor>abcd7ae66b36cc31aa12a96bb532ed145</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(juce::String)&gt;</type>
      <name>onNewsUpdate</name>
      <anchorfile>classNewsChecker.html</anchorfile>
      <anchor>a23052b426e15a02c6a09d1ea18af27ab</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>Notch12State</name>
    <filename>structNotch12State.html</filename>
    <base>FuncState</base>
    <member kind="function">
      <type></type>
      <name>Notch12State</name>
      <anchorfile>structNotch12State.html</anchorfile>
      <anchor>a2a386e7295162f4161e01a261ddd5f39</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>process</name>
      <anchorfile>structNotch12State.html</anchorfile>
      <anchor>aed8404b68201af801e4fbbf9268255f4</anchor>
      <arglist>(double v, double freq, double q)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structNotch12State.html</anchorfile>
      <anchor>a48658b560bd24c1d499838c25d07865a</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter</name>
      <anchorfile>structNotch12State.html</anchorfile>
      <anchor>a4ef2c646735b609a86004dd45130f6ff</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>Notch24State</name>
    <filename>structNotch24State.html</filename>
    <base>FuncState</base>
    <member kind="function">
      <type></type>
      <name>Notch24State</name>
      <anchorfile>structNotch24State.html</anchorfile>
      <anchor>a0f06a87bf8c3afabd6efa24ca4e3f1ed</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>process</name>
      <anchorfile>structNotch24State.html</anchorfile>
      <anchor>a3380345b2b997bfbc5e2c6aafa338310</anchor>
      <arglist>(double v, double freq, double q)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structNotch24State.html</anchorfile>
      <anchor>a5f0e8e9e830227244d942bc95496455b</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter1</name>
      <anchorfile>structNotch24State.html</anchorfile>
      <anchor>a83e85d85ead1d78d699c64f2f6afd4ec</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::IIRFilter</type>
      <name>filter2</name>
      <anchorfile>structNotch24State.html</anchorfile>
      <anchor>a26f1a87b361e824a526ddc3be30e7523</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>NoteDuration</name>
    <filename>classNoteDuration.html</filename>
    <member kind="function">
      <type></type>
      <name>NoteDuration</name>
      <anchorfile>classNoteDuration.html</anchorfile>
      <anchor>ac742ee9719f34ff0bed78e39a4f2bd4d</anchor>
      <arglist>(const char *name=&quot;&quot;, float bars=0.0f, float note=0.0f)</arglist>
    </member>
    <member kind="function">
      <type>const juce::String &amp;</type>
      <name>getName</name>
      <anchorfile>classNoteDuration.html</anchorfile>
      <anchor>a4de79765eb84ceb7204be9ab68ef0356</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>toSeconds</name>
      <anchorfile>classNoteDuration.html</anchorfile>
      <anchor>ac5647e5458ecb9b2bd6504a355d202d7</anchor>
      <arglist>(juce::AudioPlayHead *playhead) const</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>toSeconds</name>
      <anchorfile>classNoteDuration.html</anchorfile>
      <anchor>a5b06f7ea0574bbaa7a7a37451dd9667d</anchor>
      <arglist>(float bpm) const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static const std::vector&lt; NoteDuration &gt; &amp;</type>
      <name>getNoteDurations</name>
      <anchorfile>classNoteDuration.html</anchorfile>
      <anchor>acede93ced327b5fd6ce49c6042febf80</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>OpenStreetMaps</name>
    <filename>classOpenStreetMaps.html</filename>
    <class kind="class">OpenStreetMaps::Listener</class>
    <member kind="enumeration">
      <type></type>
      <name>TileSource</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OpenStreetMap</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020ad01b437a4108dcfef597b0b63ecfba77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OpenCycleMap</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020a51e83a63afd9e75954722ebacdc3573a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OpenCycleMapTransport</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020a2c24b5bde06493ad6ffa4cf85a60f1bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OpenCycleMapLandscape</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020ada2ba806323c2afdb7aa290d35450e1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>StamenTerrain</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020ad344502fa93ab8fc298bc4dbccae4c01</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MapQuestOSM</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020afcab96e000f92a339384ab2a936db4c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MapQuestOpenAerial</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020a59cb2d7d5a15e8ae207d490a2589842b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MapQuestOpenStreetMap</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020a01a650758ee32401cb56e722caa14522</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OpenStreetMap</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020ad01b437a4108dcfef597b0b63ecfba77</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OpenCycleMap</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020a51e83a63afd9e75954722ebacdc3573a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OpenCycleMapTransport</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020a2c24b5bde06493ad6ffa4cf85a60f1bc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OpenCycleMapLandscape</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020ada2ba806323c2afdb7aa290d35450e1f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>StamenTerrain</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020ad344502fa93ab8fc298bc4dbccae4c01</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MapQuestOSM</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020afcab96e000f92a339384ab2a936db4c3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MapQuestOpenAerial</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020a59cb2d7d5a15e8ae207d490a2589842b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>MapQuestOpenStreetMap</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ad685bb52fa0d8ea27d8fd8a152958020a01a650758ee32401cb56e722caa14522</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>OpenStreetMaps</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a365942b8ea69fd2f9001c9965323b252</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~OpenStreetMaps</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a42c2051ceb385127a0668acbb49e4cd0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMapTileDir</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a5d46381e37ef37f726263139d73c6072</anchor>
      <arglist>(juce::File f)</arglist>
    </member>
    <member kind="function">
      <type>juce::Image</type>
      <name>fetchTile</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a70fadcfedbacac26ed401bbb727a1280</anchor>
      <arglist>(int zoom, int x, int y)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getMapWidthPixels</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ae54d03337555816f026522ea155bce66</anchor>
      <arglist>(int zoom)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getMapWidthTiles</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a2d262f60a96e34724ce133c14a4c74b1</anchor>
      <arglist>(int zoom)</arglist>
    </member>
    <member kind="function">
      <type>juce::Point&lt; double &gt;</type>
      <name>coordinateToDisplay</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a97d05e8344c088b58ac13241d47e57b8</anchor>
      <arglist>(juce::Point&lt; double &gt; coordinate, int zoom)</arglist>
    </member>
    <member kind="function">
      <type>juce::Point&lt; double &gt;</type>
      <name>displayToCoordinate</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a2bc213d08b553d19a929faf9890f3717</anchor>
      <arglist>(const juce::Point&lt; double &gt; point, int zoom)</arglist>
    </member>
    <member kind="function">
      <type>juce::Point&lt; double &gt;</type>
      <name>tileForCoordinate</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ae835b370258746b3c52cb577e056bf3c</anchor>
      <arglist>(double lat, double lng, int zoom)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearQueue</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a4f3e70e9d0b31ca0d7c0669016da2f4f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>TileSource</type>
      <name>getTileSource</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>accb035883eacc1d62c608106455414fe</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTileSource</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a57450ff0f8965780919fd236595bbd01</anchor>
      <arglist>(TileSource t)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addListener</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>a752e32b075b5dd8490cdc8ed509ab9e1</anchor>
      <arglist>(Listener *listener)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeListener</name>
      <anchorfile>classOpenStreetMaps.html</anchorfile>
      <anchor>ab8224feb51dbb02ac20aed1664270ae6</anchor>
      <arglist>(Listener *listener)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>OscState</name>
    <filename>structOscState.html</filename>
    <base>FuncState</base>
    <member kind="function">
      <type></type>
      <name>OscState</name>
      <anchorfile>structOscState.html</anchorfile>
      <anchor>a6cdf459424576e3f91d63bc84be37b62</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>incPhase</name>
      <anchorfile>structOscState.html</anchorfile>
      <anchor>a5f7c0274cffaa5b08c431289c245ae21</anchor>
      <arglist>(float note)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>structOscState.html</anchorfile>
      <anchor>a10d53e86ac2a1192b79f2441dff5d2ef</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>phase</name>
      <anchorfile>structOscState.html</anchorfile>
      <anchor>aa4b4777463c344370332bc5ff8a141f7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>lastNote</name>
      <anchorfile>structOscState.html</anchorfile>
      <anchor>afb7d1ee26306c66e5e1f3861e771210d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>frequency</name>
      <anchorfile>structOscState.html</anchorfile>
      <anchor>ad30997798f5740bd5ae504cb75db1223</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>delta</name>
      <anchorfile>structOscState.html</anchorfile>
      <anchor>a3659847d96492e7df6bb1987b7991415</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PagedControlBox</name>
    <filename>classPagedControlBox.html</filename>
    <base>MultiParamComponent</base>
    <member kind="typedef">
      <type>ParamComponent *</type>
      <name>ParamComponentPtr</name>
      <anchorfile>classPagedControlBox.html</anchorfile>
      <anchor>afe4dfce29f71cde992d7d9449a39f269</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PagedControlBox</name>
      <anchorfile>classPagedControlBox.html</anchorfile>
      <anchor>a0993353f12db77dc373b1be89c8896bc</anchor>
      <arglist>(ProcessorEditor &amp;e)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPage</name>
      <anchorfile>classPagedControlBox.html</anchorfile>
      <anchor>a211848a46d796bfc39f5049476c0354d</anchor>
      <arglist>(const juce::String &amp;name, int rows, int cols)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPageEnable</name>
      <anchorfile>classPagedControlBox.html</anchorfile>
      <anchor>ab141e5390c0dfeb8d7585f8eeef6382a</anchor>
      <arglist>(int pageIdx, gin::Parameter::Ptr p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addBottomButton</name>
      <anchorfile>classPagedControlBox.html</anchorfile>
      <anchor>ae5cd82cbc46837688feabb580d413d19</anchor>
      <arglist>(int pageIdx, Component *c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addControl</name>
      <anchorfile>classPagedControlBox.html</anchorfile>
      <anchor>a05283324cc4c10aed959d64e5aa7011c</anchor>
      <arglist>(int pageIdx, Component *c, int x, int y, int cx=1, int cy=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPageOpen</name>
      <anchorfile>classPagedControlBox.html</anchorfile>
      <anchor>a30efdac6a995ca70b6253ec7dbb1d65a</anchor>
      <arglist>(int page, bool open)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>paramChanged</name>
      <anchorfile>classPagedControlBox.html</anchorfile>
      <anchor>a6da6768e354e21fbc3d30bfb3ed199c3</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ParamArea</name>
    <filename>classParamArea.html</filename>
    <base>MultiParamComponent</base>
    <member kind="function">
      <type></type>
      <name>ParamArea</name>
      <anchorfile>classParamArea.html</anchorfile>
      <anchor>aee22a8262ee712a259c180bacef1fdeb</anchor>
      <arglist>(const juce::String &amp;name_={})</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addControl</name>
      <anchorfile>classParamArea.html</anchorfile>
      <anchor>a574e53ae7e1feae63860723a7c7d1e28</anchor>
      <arglist>(Component *c, int x, int y, int cx=1, int cy=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addControl</name>
      <anchorfile>classParamArea.html</anchorfile>
      <anchor>a7bb2a8d10ae48495d57715ba7decd1c5</anchor>
      <arglist>(Component *c)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ParamBox</name>
    <filename>classParamBox.html</filename>
    <base>MultiParamComponent</base>
    <member kind="function">
      <type></type>
      <name>ParamBox</name>
      <anchorfile>classParamBox.html</anchorfile>
      <anchor>a011028f62a01be493f082a0e8a5f863e</anchor>
      <arglist>(const juce::String &amp;name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addEnable</name>
      <anchorfile>classParamBox.html</anchorfile>
      <anchor>a48773dc98de240a4e0599ccb490dc94b</anchor>
      <arglist>(gin::Parameter::Ptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addControl</name>
      <anchorfile>classParamBox.html</anchorfile>
      <anchor>afab51fbea411628bdb475551488cfd05</anchor>
      <arglist>(Component *c, int x, int y, int cx=1, int cy=1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addControl</name>
      <anchorfile>classParamBox.html</anchorfile>
      <anchor>a55eadc2f52821f70453b7894deb9047e</anchor>
      <arglist>(Component *c, float x, float y, float cx=1.0f, float cy=1.0f)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ParamComponent</name>
    <filename>classParamComponent.html</filename>
    <member kind="typedef">
      <type>ParamComponent *</type>
      <name>Ptr</name>
      <anchorfile>classParamComponent.html</anchorfile>
      <anchor>abe902bb7596fe1d6794047772f6f937e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ParamComponent</name>
      <anchorfile>classParamComponent.html</anchorfile>
      <anchor>a3c30a6a6391fa369bfd88533a6872ce7</anchor>
      <arglist>(Parameter *parameter)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getUid</name>
      <anchorfile>classParamComponent.html</anchorfile>
      <anchor>a62626a60b4e5e2bf86e52fa7a2362646</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTooltip</name>
      <anchorfile>classParamComponent.html</anchorfile>
      <anchor>a1e7f1920fda098d8556ab3262b6bfa8e</anchor>
      <arglist>(const juce::String &amp;newTooltip) override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Parameter *</type>
      <name>parameter</name>
      <anchorfile>classParamComponent.html</anchorfile>
      <anchor>a2e99545e9d83f8e9f2ce0c8ca31e05bd</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Parameter</name>
    <filename>classParameter.html</filename>
    <class kind="class">Parameter::ParameterListener</class>
    <class kind="struct">Parameter::ParamState</class>
    <member kind="typedef">
      <type>Parameter *</type>
      <name>Ptr</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a8fcb9478cf19509ef836db884323e5a5</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Parameter</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ae2a7ee825de84811081f393b36a5155f</anchor>
      <arglist>(Processor &amp;, juce::String uid, juce::String name, juce::String shortName, juce::String label, float minValue, float maxValue, float intervalValue, float defaultValue, float skewFactor=1.0f, std::function&lt; juce::String(const Parameter &amp;, float)&gt; textFunction=nullptr)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Parameter</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a4a449298cb4d50aa06746579e6035d1c</anchor>
      <arglist>(Processor &amp;, juce::String uid, juce::String name, juce::String shortName, juce::String label, juce::NormalisableRange&lt; float &gt; range, float defaultValue, std::function&lt; juce::String(const Parameter &amp;, float)&gt; textFunction=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getUid</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a1541ad0a9159f2433a6e652196b9d377</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setInternal</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ad521876299da1e44dfb839ebb13040c3</anchor>
      <arglist>(bool i)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isInternal</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>afb5c784fc967bb5c8cb092ca08be94cf</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setModIndex</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>abcea7fddcf44942934220143f798617f</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getModIndex</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a993beb638d6020998362efb7523f818d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setModMatrix</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a88158bf1dd98daead5fc2215e17c4d3d</anchor>
      <arglist>(ModMatrix *m)</arglist>
    </member>
    <member kind="function">
      <type>ModMatrix *</type>
      <name>getModMatrix</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a659b6ef154cdff127cc36c0af6cfcdf1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>prepareToPlay</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ae91e3ee8425847a19c3b158a26f76896</anchor>
      <arglist>(double, int)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>reset</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a0ec10eb10932870b40ee91f8c25268e3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getProcValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a97f748fad458972d722789fd47d6adfa</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual float</type>
      <name>getProcValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a3307e2e5d9d7ee73567e8fdb355adfa3</anchor>
      <arglist>(int stepSize)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getUserValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>afb03893bbc54670d3861d07392f555ee</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getUserValueInt</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ac26990761b124bea267b591544eded45</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getUserValueBool</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>af4df6ea9cf75227873e98ee076c89020</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getUserDefaultValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>acb293e0243a02b21a0ef99a7a2f2d900</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setUserValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a4aad90b86dc49666d345b8f0e01c0347</anchor>
      <arglist>(float v)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setUserValueNotifingHost</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ab255a9f572a46486d9c01dbb9e3137b3</anchor>
      <arglist>(float f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setUserValueAsUserAction</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a4d6386eea25fad5670125701b537f858</anchor>
      <arglist>(float f)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getUserValueText</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a202a2145b5b03a4ad6622718e49557ab</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>userValueToText</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>af3960250a1c071e471daf16be484b656</anchor>
      <arglist>(float val)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isOnOff</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ab6d9770682352a532ef81cbdbbbf8e8b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isOn</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a372ed65aedcc9fff511bba20b28687c8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual bool</type>
      <name>isSmoothingActive</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a4d02647baeee12be33b39e013810ee12</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>beginUserAction</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a4835756858c197a20d3eeb1e88d11f7d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>beginUserTimedAction</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>aa76ca1fc6f05803166264b33e0546bf4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>endUserAction</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ad1a476e84eb995f1f39073ee156e78a1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::NormalisableRange&lt; float &gt;</type>
      <name>getUserRange</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ab12bfdd7bebce174eacad15681cf37d8</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getUserRangeStart</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a15114ceeb53ebd72469dd3a8a4d9a154</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getUserRangeEnd</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a1dd196a1f6c578c025b62159397d8db4</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addListener</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ac51a0648ba9ea0581dd667b25ee6e31a</anchor>
      <arglist>(ParameterListener *listener)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeListener</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a2efe9da5477dbd10bcd7d6073dde5823</anchor>
      <arglist>(ParameterListener *listener)</arglist>
    </member>
    <member kind="function">
      <type>ParamState</type>
      <name>getState</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a5a30a4d6e5fb11e10fd040227f1a08e4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setState</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a73922c94ec8df3090b2bcba4c0c931eb</anchor>
      <arglist>(const ParamState &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getParameterID</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a316738b42194e1359158933b8dc11417</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>afc59f687d30b5816cdfb2aaace49a907</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getBoolValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a9bc8e183ac8f271dbc8c4ee0aff647e5</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ac5ad890a0df82d560e2a94ecd5d1df1f</anchor>
      <arglist>(float newValue) override</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getDefaultValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a400ebf8ccc781cbdcfcbf305dcec5f92</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getName</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a41f6a2e45cc127a47e751ef6f3892f77</anchor>
      <arglist>(int maximumStringLength) const override</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getShortName</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a92105e6395bf23748f2c61d1c76f2fdd</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getLabel</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>aa2b511b55b1edb8eaa5cc3cb5a1a1e30</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumSteps</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a1b51959c06800d37563cb72001640014</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getText</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>aeadd7715c99d8d8b788e5800d620104e</anchor>
      <arglist>(float value, int) const override</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getValueForText</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a9019aebfa160ab78e1884be19ba59a67</anchor>
      <arglist>(const juce::String &amp;text) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isOrientationInverted</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>aaf1d7ae11ba787ef0fd6595f0fd1c9aa</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isAutomatable</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>aee892abdf7a0df0dd2a4ba4f7371a6a6</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isMetaParameter</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ac7b97ec3abd89250cc2e723e0c4493bc</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getSkew</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a424a9513d94c87b8d101ae0017f3d71c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isSkewSymmetric</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>ad14d3f665f2beb89e988f48db3d45e8e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; float(float)&gt;</type>
      <name>conversionFunction</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>aec8da4e81f800c8e35e40a6700474c10</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>handleAsyncUpdate</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a8296a6e53d729655e66f2cce22dc7924</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>timerCallback</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a8d5e385c39a92ffcede0ce5476451e16</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>changed</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a5ddd8918e54589b570925d7530fd2943</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Processor &amp;</type>
      <name>processor</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>aff54b112e093cf966a1bec5bf5a934dd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::NormalisableRange&lt; float &gt;</type>
      <name>range</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a4d0ec755973b5650a80d2b7e39bc2197</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>internal</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a25165e96742e860d5ee0e8a6f70a69f5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>ModMatrix *</type>
      <name>modMatrix</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a30f079fa8ecf5c5cd5735777e1ba74c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>modIndex</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a7310a6224d36ab27a14237aab7d8fadd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>value</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a600fbf5505f6846a37b27f806ea4abf0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>float</type>
      <name>defaultValue</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>afff117f671e74a238989542b73cbd024</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::String</type>
      <name>uid</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a6de74e7339f55349e6cb89dfd9a3abf1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::String</type>
      <name>name</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a4316d020a490d5e1d764d60453cb078c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::String</type>
      <name>shortName</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a13e74b870f06935303747b3921f80dfa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::String</type>
      <name>label</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a18023bfac5c58f4372050d2a8177006e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::function&lt; juce::String(const Parameter &amp;, float)&gt;</type>
      <name>textFunction</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a53a9fdfe21f1bfa1a41c985335ad76aa</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>userActionCount</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a9a2d682ac453e9b019ceb12986b8e5fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::ListenerList&lt; ParameterListener &gt;</type>
      <name>listeners</name>
      <anchorfile>classParameter.html</anchorfile>
      <anchor>a6ab022db2c42737bf0d3b1552f9b97a4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Parameter::ParameterListener</name>
    <filename>classParameter_1_1ParameterListener.html</filename>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~ParameterListener</name>
      <anchorfile>classParameter_1_1ParameterListener.html</anchorfile>
      <anchor>a3419172cb5e87763490dafae52f5d39f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>valueUpdated</name>
      <anchorfile>classParameter_1_1ParameterListener.html</anchorfile>
      <anchor>aeb8d451cc143b571fbf0edffc8e47266</anchor>
      <arglist>(Parameter *param)=0</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>LFO::Parameters</name>
    <filename>structLFO_1_1Parameters.html</filename>
    <member kind="variable">
      <type>WaveShape</type>
      <name>waveShape</name>
      <anchorfile>structLFO_1_1Parameters.html</anchorfile>
      <anchor>a5128a3dd3bf6a99e901bff2e962bdc81</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>frequency</name>
      <anchorfile>structLFO_1_1Parameters.html</anchorfile>
      <anchor>a69abbc2714aacf8d32fc515b46c39c1e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>phase</name>
      <anchorfile>structLFO_1_1Parameters.html</anchorfile>
      <anchor>aa952c923e064d33a4061f6c3e49c3d7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>offset</name>
      <anchorfile>structLFO_1_1Parameters.html</anchorfile>
      <anchor>aa98b00b462d53ce4b331ed6b9c0d6dd9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>depth</name>
      <anchorfile>structLFO_1_1Parameters.html</anchorfile>
      <anchor>aea5b1c1cd4fc3b68d3826fce64f6287a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>delay</name>
      <anchorfile>structLFO_1_1Parameters.html</anchorfile>
      <anchor>a9be35ed3aca74afab18de36202cdfd97</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>fade</name>
      <anchorfile>structLFO_1_1Parameters.html</anchorfile>
      <anchor>a20d76d843455f85c2131d9ed77329499</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ParamHeader</name>
    <filename>classParamHeader.html</filename>
    <member kind="function">
      <type></type>
      <name>ParamHeader</name>
      <anchorfile>classParamHeader.html</anchorfile>
      <anchor>a79840eb71c893f1273cfd46e4f6ccda5</anchor>
      <arglist>(const juce::String &amp;name_)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>StereoOscillator::Params</name>
    <filename>structStereoOscillator_1_1Params.html</filename>
    <member kind="variable">
      <type>Wave</type>
      <name>wave</name>
      <anchorfile>structStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a20a0ac6dcf991d68183208a99d95ea02</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>leftGain</name>
      <anchorfile>structStereoOscillator_1_1Params.html</anchorfile>
      <anchor>ab7814fe28bdb79772871cef3cf5f5679</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>rightGain</name>
      <anchorfile>structStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a5c18505090d6b512295acdb7aa2ae707</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>pw</name>
      <anchorfile>structStereoOscillator_1_1Params.html</anchorfile>
      <anchor>aebc2ccd97a83304620840c479a8c782b</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>VoicedStereoOscillator::Params</name>
    <filename>structVoicedStereoOscillator_1_1Params.html</filename>
    <member kind="variable">
      <type>Wave</type>
      <name>wave</name>
      <anchorfile>structVoicedStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a83a9e522b4761797d6bf5b90ba610f41</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>voices</name>
      <anchorfile>structVoicedStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a46ae813a8ece08ac931fbb1b6b3adcfd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>vcTrns</name>
      <anchorfile>structVoicedStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a617f4529f6385f4cc4cf74516e3bdc25</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>pw</name>
      <anchorfile>structVoicedStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a120aa15f174feb6b06c66d2f76103bf3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>pan</name>
      <anchorfile>structVoicedStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a13309716133330853a2ad479b2b556dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>spread</name>
      <anchorfile>structVoicedStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a1b592edb5ad2668d86c1abba77f0b6fd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>detune</name>
      <anchorfile>structVoicedStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a3f1b408a97910ce6a63de0a8073c85c7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>gain</name>
      <anchorfile>structVoicedStereoOscillator_1_1Params.html</anchorfile>
      <anchor>a11ed6981f0f7ee29502bc00b3c628fe4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>WTOscillator::Params</name>
    <filename>structWTOscillator_1_1Params.html</filename>
    <member kind="variable">
      <type>Wave</type>
      <name>wave</name>
      <anchorfile>structWTOscillator_1_1Params.html</anchorfile>
      <anchor>ab5232b4b55cc4d6b076db62478cd436e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>leftGain</name>
      <anchorfile>structWTOscillator_1_1Params.html</anchorfile>
      <anchor>abef23350d10d65198a293119ced8bb9d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>rightGain</name>
      <anchorfile>structWTOscillator_1_1Params.html</anchorfile>
      <anchor>a432702cbb02f4013a3915a03ca672c39</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>pw</name>
      <anchorfile>structWTOscillator_1_1Params.html</anchorfile>
      <anchor>a7ca54915f07cb3e86bc516fa5710b71f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>Parameter::ParamState</name>
    <filename>structParameter_1_1ParamState.html</filename>
    <member kind="variable">
      <type>juce::String</type>
      <name>uid</name>
      <anchorfile>structParameter_1_1ParamState.html</anchorfile>
      <anchor>aa22b9ef79902c319f80799bbaae5c9e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>value</name>
      <anchorfile>structParameter_1_1ParamState.html</anchorfile>
      <anchor>a9688db68c2a8bd8fc608720e62ea9c8a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PatchBrowser</name>
    <filename>classPatchBrowser.html</filename>
    <class kind="struct">PatchBrowser::AuthorsModel</class>
    <class kind="struct">PatchBrowser::PresetsModel</class>
    <class kind="struct">PatchBrowser::TagsModel</class>
    <member kind="function">
      <type></type>
      <name>PatchBrowser</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a4beb28a79d33b3385cd036e78166443c</anchor>
      <arglist>(Processor &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateSelection</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a63f6aec5fdb704d05593cac1823bb9bb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>refresh</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a1d7217de51f2b36702575561de21fee6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resized</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a1b458409720f15dc6e8629c3402a0968</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>af622c9352f699990659deda9230a5900</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="variable">
      <type>Processor &amp;</type>
      <name>proc</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>ace4451a4ee2e4ea777e795e8913ae29b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>AuthorsModel</type>
      <name>authorsModel</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a62a7cf5ab56615e84109ced00beb6dcb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>TagsModel</type>
      <name>tagsModel</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a2b1148951ac60c4b47c4b1f6af16d144</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>PresetsModel</type>
      <name>presetsModel</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>acce838df5fbbd1505ae5c0b55d361604</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::ListBox</type>
      <name>authors</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a1becbf2ecebca6e5a2ba2533b619b952</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::ListBox</type>
      <name>tags</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>aa7bac749585a5be93c25474dae4542e8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::ListBox</type>
      <name>presets</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a2fde0c9a4e9504c0feb276e93250ea12</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::StringArray</type>
      <name>currentAuthors</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a6706171e059f0acd5c48aded8185378b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::StringArray</type>
      <name>currentTags</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>ad6b24119439bd1a1feafc67751a4c965</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::StringArray</type>
      <name>currentPresets</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a6f234b2f7429023b795fca3aeefb4347</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::StringArray</type>
      <name>selectedAuthors</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a9b9d7b139ae59f19b0b3435dba2ce027</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::StringArray</type>
      <name>selectedTags</name>
      <anchorfile>classPatchBrowser.html</anchorfile>
      <anchor>a5131ad5197cd707f759b87775770551f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PerlinNoise</name>
    <filename>classPerlinNoise.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>PerlinNoise</name>
      <anchorfile>classPerlinNoise.html</anchorfile>
      <anchor>a72f70e187f3744ea81b57003f4dd886f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>PerlinNoise</name>
      <anchorfile>classPerlinNoise.html</anchorfile>
      <anchor>a550e04d7866770775689d81e1d99461f</anchor>
      <arglist>(unsigned int seed)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>noise</name>
      <anchorfile>classPerlinNoise.html</anchorfile>
      <anchor>a3aa782c396ab798493ad7529960c5a9a</anchor>
      <arglist>(T x, T y=0, T z=0)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PluginAlertWindow</name>
    <filename>classPluginAlertWindow.html</filename>
    <member kind="function">
      <type></type>
      <name>PluginAlertWindow</name>
      <anchorfile>classPluginAlertWindow.html</anchorfile>
      <anchor>a25f98216ebdc7b63e16471784b05e12b</anchor>
      <arglist>(const juce::String &amp;title, const juce::String &amp;message, AlertIconType iconType, Component *associatedComponent=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>runModalLoop</name>
      <anchorfile>classPluginAlertWindow.html</anchorfile>
      <anchor>a6869a64a9ff0b4da3319bcb3c093219d</anchor>
      <arglist>(Component &amp;parent)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PluginButton</name>
    <filename>classPluginButton.html</filename>
    <base protection="private">Parameter::ParameterListener</base>
    <member kind="function">
      <type></type>
      <name>PluginButton</name>
      <anchorfile>classPluginButton.html</anchorfile>
      <anchor>ae01c3f3ee210cdbecc5e20545edf743d</anchor>
      <arglist>(Parameter *parameter_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~PluginButton</name>
      <anchorfile>classPluginButton.html</anchorfile>
      <anchor>ad6caf112e78c313d9c24d1606f0ea803</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>valueUpdated</name>
      <anchorfile>classPluginButton.html</anchorfile>
      <anchor>aa764a81b1678f58f8034b9df753c8b77</anchor>
      <arglist>(Parameter *) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clicked</name>
      <anchorfile>classPluginButton.html</anchorfile>
      <anchor>a5a8bc6d21639253463b285091b3ff1e3</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>parentHierarchyChanged</name>
      <anchorfile>classPluginButton.html</anchorfile>
      <anchor>a018349067cbf3587776075d91abe657d</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>Parameter *</type>
      <name>parameter</name>
      <anchorfile>classPluginButton.html</anchorfile>
      <anchor>ae383af96f9f3e0cf932e0eb63116257a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PluginComboBox</name>
    <filename>classPluginComboBox.html</filename>
    <base protection="private">Parameter::ParameterListener</base>
    <member kind="function">
      <type></type>
      <name>PluginComboBox</name>
      <anchorfile>classPluginComboBox.html</anchorfile>
      <anchor>aa690d48460e1929b21424f44ae8fddce</anchor>
      <arglist>(Parameter *parameter_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~PluginComboBox</name>
      <anchorfile>classPluginComboBox.html</anchorfile>
      <anchor>a9e3e9c7e0f9aa97d38151eb43f0e618a</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>valueUpdated</name>
      <anchorfile>classPluginComboBox.html</anchorfile>
      <anchor>a3974da62403031912ad0c232dafbf8dc</anchor>
      <arglist>(Parameter *) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>comboBoxChanged</name>
      <anchorfile>classPluginComboBox.html</anchorfile>
      <anchor>a9681caf99eafeacbbd43fa83a7910fbd</anchor>
      <arglist>(ComboBox *) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>parentHierarchyChanged</name>
      <anchorfile>classPluginComboBox.html</anchorfile>
      <anchor>ae276b856d377f7d36351a2a8205e2b99</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable">
      <type>Parameter *</type>
      <name>parameter</name>
      <anchorfile>classPluginComboBox.html</anchorfile>
      <anchor>a3bbf40da531c73ccefc9374433ae9d88</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PluginLookAndFeel</name>
    <filename>classPluginLookAndFeel.html</filename>
    <base>GinLookAndFeel</base>
    <member kind="function">
      <type></type>
      <name>PluginLookAndFeel</name>
      <anchorfile>classPluginLookAndFeel.html</anchorfile>
      <anchor>ae6503ae3672e0e5e5517fb9d9f4de9e5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::Typeface::Ptr</type>
      <name>getTypefaceForFont</name>
      <anchorfile>classPluginLookAndFeel.html</anchorfile>
      <anchor>a2039e6dc6cf25994545f54527cddc9cc</anchor>
      <arglist>(const juce::Font &amp;f) override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PluginLookAndFeelWrapper</name>
    <filename>classPluginLookAndFeelWrapper.html</filename>
    <base>PluginLookAndFeel</base>
    <member kind="function">
      <type></type>
      <name>PluginLookAndFeelWrapper</name>
      <anchorfile>classPluginLookAndFeelWrapper.html</anchorfile>
      <anchor>a53db8e2558505885fbb70b502c64cc78</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~PluginLookAndFeelWrapper</name>
      <anchorfile>classPluginLookAndFeelWrapper.html</anchorfile>
      <anchor>a016e06286dc0edfff487f85d5d6fa1a7</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PluginSlider</name>
    <filename>classPluginSlider.html</filename>
    <base protection="private">Parameter::ParameterListener</base>
    <member kind="function">
      <type></type>
      <name>PluginSlider</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>aa55574ec47ca63deb3b26a11d1eb8135</anchor>
      <arglist>(Parameter *parameter_, SliderStyle style, TextEntryBoxPosition textBoxPosition)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~PluginSlider</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>a4010b75e431d5c17e144c6387ea71664</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sliderValueChanged</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>ac566200d8e55e399a865bb74a87c1fb5</anchor>
      <arglist>(Slider *) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sliderDragStarted</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>a4d84b0d9bb52c3a5ffa4e08ee6d76cd2</anchor>
      <arglist>(Slider *) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sliderDragEnded</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>ab7c8653f65c5c80e45266df4370208ec</anchor>
      <arglist>(Slider *) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>valueUpdated</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>af47461ce84833d9e5ab38b459c42c494</anchor>
      <arglist>(Parameter *) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mouseDown</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>ae74825b59e7dc1c6f654488876213527</anchor>
      <arglist>(const juce::MouseEvent &amp;e) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mouseUp</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>aa2eff100b1ed428b91f0ef8b6fb26e99</anchor>
      <arglist>(const juce::MouseEvent &amp;e) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mouseDrag</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>a564ef68cc52df0d872231fd0c5446fd7</anchor>
      <arglist>(const juce::MouseEvent &amp;e) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>parentHierarchyChanged</name>
      <anchorfile>classPluginSlider.html</anchorfile>
      <anchor>a85064398e0b6fb0387554a4c5877fa67</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Point</name>
    <filename>classPoint.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>Point</name>
      <anchorfile>classPoint.html</anchorfile>
      <anchor>ae6a75fcca567819ab05482257ce36bdc</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Point</name>
      <anchorfile>classPoint.html</anchorfile>
      <anchor>a9853b0631fc1e4c3ec3fab8ac26314a9</anchor>
      <arglist>(T x_, T y_)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getX</name>
      <anchorfile>classPoint.html</anchorfile>
      <anchor>aed1f0a89874a790ac96175180c934f43</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getY</name>
      <anchorfile>classPoint.html</anchorfile>
      <anchor>af2d83c0f10795ba0393e7a8b86c0719a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>T</type>
      <name>x</name>
      <anchorfile>classPoint.html</anchorfile>
      <anchor>a401d07562afaf0079121218025e66b76</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>T</type>
      <name>y</name>
      <anchorfile>classPoint.html</anchorfile>
      <anchor>a65146418a33ebb2cd9acb85cade60ac9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PowerButton</name>
    <filename>classPowerButton.html</filename>
    <base>PluginButton</base>
    <member kind="function">
      <type></type>
      <name>PowerButton</name>
      <anchorfile>classPowerButton.html</anchorfile>
      <anchor>a1bc34438020441f421334d0460f325e9</anchor>
      <arglist>(Parameter *p)</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>PatchBrowser::PresetsModel</name>
    <filename>structPatchBrowser_1_1PresetsModel.html</filename>
    <member kind="function">
      <type></type>
      <name>PresetsModel</name>
      <anchorfile>structPatchBrowser_1_1PresetsModel.html</anchorfile>
      <anchor>a786c7135368bcbd2fdeef02f1789c597</anchor>
      <arglist>(PatchBrowser &amp;o)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumRows</name>
      <anchorfile>structPatchBrowser_1_1PresetsModel.html</anchorfile>
      <anchor>a44f9a1678a67661480ea71ab7a0a0d4d</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paintListBoxItem</name>
      <anchorfile>structPatchBrowser_1_1PresetsModel.html</anchorfile>
      <anchor>a09fc3d69b6560cbe3c9992b06b7f3637</anchor>
      <arglist>(int row, juce::Graphics &amp;g, int w, int h, bool selected) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>listBoxItemDoubleClicked</name>
      <anchorfile>structPatchBrowser_1_1PresetsModel.html</anchorfile>
      <anchor>a8cc3194c2da68f867bcf24aa78384bcd</anchor>
      <arglist>(int row, const juce::MouseEvent &amp;) override</arglist>
    </member>
    <member kind="variable">
      <type>PatchBrowser &amp;</type>
      <name>owner</name>
      <anchorfile>structPatchBrowser_1_1PresetsModel.html</anchorfile>
      <anchor>a1c3950e8b62521c5bbb278048a5b3e37</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Processor</name>
    <filename>classProcessor.html</filename>
    <member kind="function">
      <type></type>
      <name>Processor</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>ab4b297f2d055f3316918d3e23081a45a</anchor>
      <arglist>(bool init=true)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Processor</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a3d7c16d172be09d3bfb4df3519d21b31</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>init</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a22e869ee49d974ad0ee7ee81961ab88f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>aea94295621f428e66e1d2fc7bfb39bb6</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>prepareToPlay</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a91501bb29c8b65f444460ed1abc22824</anchor>
      <arglist>(double sampleRate, int samplesPerBlock) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual std::unique_ptr&lt; juce::PropertiesFile &gt;</type>
      <name>getSettings</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a78cb0aaf6f673ed95a5c32e60ae93427</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addPluginParameter</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a4b2ed4cc27b858a3aac157c05e7b273e</anchor>
      <arglist>(gin::Parameter *parameter)</arglist>
    </member>
    <member kind="function">
      <type>gin::Parameter *</type>
      <name>addExtParam</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>ad6b6443808b166f6194d07bf68a6e5f7</anchor>
      <arglist>(juce::String uid, juce::String name, juce::String shortName, juce::String label, juce::NormalisableRange&lt; float &gt; range, float defaultValue, SmoothingType st, std::function&lt; juce::String(const gin::Parameter &amp;, float)&gt; textFunction=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>gin::Parameter *</type>
      <name>addIntParam</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a3e19869a9d96d0ee6492bdec952bab2d</anchor>
      <arglist>(juce::String uid, juce::String name, juce::String shortName, juce::String label, juce::NormalisableRange&lt; float &gt; range, float defaultValue, SmoothingType st, std::function&lt; juce::String(const gin::Parameter &amp;, float)&gt; textFunction=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>gin::Parameter *</type>
      <name>getParameter</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a172c59635f441ade54855f4041046d8b</anchor>
      <arglist>(const juce::String &amp;uid)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>parameterValue</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>afdd29a9ae5ebcda535a8e0758aad92b4</anchor>
      <arglist>(const juce::String &amp;uid)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>parameterIntValue</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a6636672f1d6413c06be3aced7622731d</anchor>
      <arglist>(const juce::String &amp;uid)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>parameterBoolValue</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a980e98e084a98f9fdaa3c958c613a21f</anchor>
      <arglist>(const juce::String &amp;uid)</arglist>
    </member>
    <member kind="function">
      <type>const juce::Array&lt; gin::Parameter * &gt; &amp;</type>
      <name>getPluginParameters</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a2d76202bddbb9f54afbb7f10dcf2a3c6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isSmoothing</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a9bbb09bf94621a6d29aff0c54ce4029f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual juce::File</type>
      <name>getProgramDirectory</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a36394e5ceac103d0f3f7852091aad91e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>loadAllPrograms</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a0d54dc4f330cea0f907e55a4f19e92d3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const juce::String</type>
      <name>getName</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a39f7bb4cef80c8e49f3220605f4cc455</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>acceptsMidi</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>aa945a9fb36430295388fc9973e761c21</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>producesMidi</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a849cf5d39d72d99ec8b962e237f363e2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getTailLengthSeconds</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a5c9121d197fae93897d4faffe637e8f2</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumPrograms</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>aa9544d1b18b0bc03c32fa78532de316b</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getCurrentProgram</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a2d74b270efcff268d88dc9895d3a0e83</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>const juce::OwnedArray&lt; Program &gt; &amp;</type>
      <name>getPrograms</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a04c4c0dca9f2560f99adea0ee7618db8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCurrentProgram</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a16740174d6c17303dfc77ef45d32f348</anchor>
      <arglist>(juce::String name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCurrentProgram</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>adc86771c8cf8ebf5dc33a3c296216c58</anchor>
      <arglist>(int index) override</arglist>
    </member>
    <member kind="function">
      <type>const juce::String</type>
      <name>getProgramName</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>ae58f2ae2b4b20a3fa4c9981df3e336d3</anchor>
      <arglist>(int index) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>changeProgramName</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a912ce9ee0afb3cc9b48804cd5566f1ea</anchor>
      <arglist>(int index, const juce::String &amp;newName) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveProgram</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a964be03f12126fcf8a9c5996af2abbef</anchor>
      <arglist>(juce::String name, juce::String author, juce::String tags)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>deleteProgram</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>aac9dc03295d9a5a991bc725d9f4bbde7</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>hasProgram</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>ad78ac9883a5b0ccabf449861aa86c071</anchor>
      <arglist>(juce::String name)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>getStateInformation</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>ad2e33e15dd248d9478b4a55d8eb5e36f</anchor>
      <arglist>(juce::MemoryBlock &amp;destData) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setStateInformation</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a11507c2da1e031b89a1cf74aa4f00888</anchor>
      <arglist>(const void *data, int sizeInBytes) override</arglist>
    </member>
    <member kind="variable">
      <type>std::unique_ptr&lt; PluginLookAndFeel &gt;</type>
      <name>lf</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a756a634ec593cbe38c40f95e6554ee04</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::map&lt; juce::String, gin::Parameter * &gt;</type>
      <name>parameterMap</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a0eed0ccb9cccd2bc8a0d218fc42488b5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::OwnedArray&lt; gin::Parameter &gt;</type>
      <name>internalParameters</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>ac9d3cf083da1dba0b2612ce32ea7218d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::ValueTree</type>
      <name>state</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>aecd42420782c38cad73bf06fb0f195db</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>stateUpdated</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>a8f2a4e8408ed85ccf02152f5e0cc4143</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>updateState</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>ab477f2a48ee23cb7d38dbecd87332fcd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>extractProgram</name>
      <anchorfile>classProcessor.html</anchorfile>
      <anchor>afdf6943c2323aeedf39c18d7ec77ecbb</anchor>
      <arglist>(const juce::String &amp;name, const juce::MemoryBlock &amp;data)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ProcessorEditor</name>
    <filename>classProcessorEditor.html</filename>
    <base>ProcessorEditorBase</base>
    <member kind="function">
      <type></type>
      <name>ProcessorEditor</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a1243029ef3210c55622ab215bb90d378</anchor>
      <arglist>(Processor &amp;) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ProcessorEditor</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>af0945c5f7f341f83fb7ff667696d03fb</anchor>
      <arglist>(Processor &amp;, int cx, int cy) noexcept</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ProcessorEditor</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a6ff6d71c70b97410c0b4c055012f5dff</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>showAboutInfo</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a2d881e0cfe4ab6df47e5ae0c5a86cf52</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>refreshPatchBrowser</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a29115b399e4bf3d6bcd27652d6bb1ada</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>showPatchBrowser</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>abcef3707e7363c0b7ae0877c64542d26</anchor>
      <arglist>(bool p)</arglist>
    </member>
    <member kind="variable">
      <type>Processor &amp;</type>
      <name>slProc</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a8f4bf0f17d18d56fe21c845a41e1cb7e</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a725f16140eeba1f2f996c46d4aa353f5</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>resized</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>aab2a5800a71626b039a05dac4ec771a0</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>refreshPrograms</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>acfa6525c7af7ab451c59d25527ddebde</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; UpdateChecker &gt;</type>
      <name>updateChecker</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>adb1321c64dc3799c8b5b7128ec7fdae1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; NewsChecker &gt;</type>
      <name>newsChecker</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a7bcb131b783c30e7fcc661bc23699601</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::String</type>
      <name>additionalProgramming</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a9139785557b0904a5b52aab9b4b47e22</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>TitleBar</type>
      <name>titleBar</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a765fed46b8f886ab4e6306124110ed68</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>PatchBrowser</type>
      <name>patchBrowser</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a6f7f6e1a092dff5bccece83325514c77</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::String</type>
      <name>extraProgrammer</name>
      <anchorfile>classProcessorEditor.html</anchorfile>
      <anchor>a9f8c53b3bc09b12d34a17c9da2aaf32d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ProcessorEditorBase</name>
    <filename>classProcessorEditorBase.html</filename>
    <member kind="function">
      <type></type>
      <name>ProcessorEditorBase</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a4aa0f87f4268ac1cb146bdc71376cc89</anchor>
      <arglist>(Processor &amp;p, int cx_, int cy_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ProcessorEditorBase</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a31331076e669e82290048f36e689e439</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>makeResizable</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>ae96b66e386ddc6abe2ac208987f781c2</anchor>
      <arglist>(int minX, int minY, int maxX, int maxY)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resized</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>aeb529daa7c8a33c654615fc31240670d</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>globalFocusChanged</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a534df390c7aa61933255e6397546234e</anchor>
      <arglist>(juce::Component *) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paintOverChildren</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a3043bfdfbefb8e16952f471b092150e1</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual juce::Rectangle&lt; int &gt;</type>
      <name>getControlsArea</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a2a5d14bd76e1051ce255e5033a2b1f1a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual juce::Rectangle&lt; int &gt;</type>
      <name>getGridArea</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>af1d97ab85b7b4eaed80e1cd1f9c704ee</anchor>
      <arglist>(int x, int y, int w=1, int h=1)</arglist>
    </member>
    <member kind="function">
      <type>juce::Rectangle&lt; int &gt;</type>
      <name>getFullGridArea</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>acf57a677075ccf23cc6c253dc74b7ca6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getGridWidth</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>ae0f5b7056efe5b77a00484cad4d70462</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getGridHeight</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a93c1337e016ea549c34bc7448370577b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>getUseIncreasedKeyboardAccessibility</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>aa55bc190267e15e007544badd68830cb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setUseIncreasedKeyboardAccessibility</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a2f1f906db82e4a9741e3916a8a6c7451</anchor>
      <arglist>(bool accessible)</arglist>
    </member>
    <member kind="variable">
      <type>juce::ComponentBoundsConstrainer</type>
      <name>resizeLimits</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>aa6662122b927e35fb5adf36cf9d79dcf</anchor>
      <arglist></arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>setGridSize</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>aae5f9927723bd09656437199e4c625b2</anchor>
      <arglist>(int x, int y, int extraWidthPx=0, int extraHeightPx=0)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>ParamComponent *</type>
      <name>componentForId</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>ac3e74209225daebb55e7b4262f2711da</anchor>
      <arglist>(const juce::String &amp;uid)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>ParamComponent *</type>
      <name>componentForParam</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>abbf6549241813d270e584d61653c2c55</anchor>
      <arglist>(Parameter &amp;param)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Processor &amp;</type>
      <name>ginProcessor</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>abea55336b0e8f3fe0a6392e288ea27bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; juce::ResizableCornerComponent &gt;</type>
      <name>resizer</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a852dc8320bd7a08b2c2cc4214cb7ec23</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>const int</type>
      <name>cx</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a1e7e8587520931e79cbd2c90ab2122bf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>const int</type>
      <name>cy</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a2c1c1c5590ca8e090ba362a395b339eb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::OwnedArray&lt; ParamComponent &gt;</type>
      <name>controls</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a34eb37b1f9a5896e6f5ea9244993d7bd</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>headerHeight</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>abc8ed784a5fcb7e1efe3f1dc63348f82</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>inset</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>aa1ef9037d52f21d67f16efa2f7b7bb1b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>cols</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a69d51f7716435429bfdcd1547105fa2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>rows</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a0d3a93feb2d42d9424386d5e21660b1a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>extraWidthPx</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a4c568cb0377c6626240acc5eaf5fff2a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>extraHeightPx</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>a563ccdce57a82178551cd6e26d601d27</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::TooltipWindow</type>
      <name>tooltipWindow</name>
      <anchorfile>classProcessorEditorBase.html</anchorfile>
      <anchor>aef611bd53bf795b5e0e9b204febd4070</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Program</name>
    <filename>classProgram.html</filename>
    <member kind="function">
      <type></type>
      <name>Program</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>ac702b6b062bd2b1aa35003f76a34f2a5</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>loadProcessor</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>a9b90c3645e8e2cad9f006a2373901e2c</anchor>
      <arglist>(Processor &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveProcessor</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>a6f805c78d2f7a4ffbd0a98b6435eef0a</anchor>
      <arglist>(Processor &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>loadFromFile</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>a00c563471f7d8b81fd534193c0baa6d6</anchor>
      <arglist>(juce::File f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveToDir</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>a72eeaa3511a33b3d4bbb7b480029f9ae</anchor>
      <arglist>(juce::File f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>deleteFromDir</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>a036687ead06a204df0e772821bc861bf</anchor>
      <arglist>(juce::File f)</arglist>
    </member>
    <member kind="variable">
      <type>juce::String</type>
      <name>name</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>ab960dff0d490441b19a47c7788ef5308</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::String</type>
      <name>author</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>a98dafb9ef18422b1a03261697b874194</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::StringArray</type>
      <name>tags</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>acddd8235091f27410c05cf4b7694b62a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::String</type>
      <name>valueTree</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>ae5df2a7c91a9814b63c9224106492da4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::Array&lt; Parameter::ParamState &gt;</type>
      <name>states</name>
      <anchorfile>classProgram.html</anchorfile>
      <anchor>ac96aecc2feeb808135496ea32108a3f1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>PropertyComponentBase</name>
    <filename>classPropertyComponentBase.html</filename>
    <member kind="function">
      <type></type>
      <name>PropertyComponentBase</name>
      <anchorfile>classPropertyComponentBase.html</anchorfile>
      <anchor>ae320b0e3de35b2ead852cbc5daa5d5f7</anchor>
      <arglist>(const juce::Value &amp;valueToControl, const juce::String &amp;propertyName)</arglist>
    </member>
    <member kind="function">
      <type>juce::Value &amp;</type>
      <name>getValueObject</name>
      <anchorfile>classPropertyComponentBase.html</anchorfile>
      <anchor>a032611b29e0d7969984b5edbd3bf062d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>valueChanged</name>
      <anchorfile>classPropertyComponentBase.html</anchorfile>
      <anchor>af34efe7a5ff3eb5199e3f06fce52b6ff</anchor>
      <arglist>(juce::Value &amp;) override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::Value</type>
      <name>value</name>
      <anchorfile>classPropertyComponentBase.html</anchorfile>
      <anchor>a3c5ee7d20c4aecc48394fc97aa050448</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Readout</name>
    <filename>classReadout.html</filename>
    <base protection="private">Parameter::ParameterListener</base>
    <member kind="function">
      <type></type>
      <name>Readout</name>
      <anchorfile>classReadout.html</anchorfile>
      <anchor>a06ee0c5ad849b4d2940bd31d3a81eb6e</anchor>
      <arglist>(Parameter *parameter)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Readout</name>
      <anchorfile>classReadout.html</anchorfile>
      <anchor>a62ebff9c5a4eae6c22f5a65992d902af</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RealtimeAsyncUpdater</name>
    <filename>classRealtimeAsyncUpdater.html</filename>
    <member kind="function">
      <type></type>
      <name>RealtimeAsyncUpdater</name>
      <anchorfile>classRealtimeAsyncUpdater.html</anchorfile>
      <anchor>a8cba7dff87861e17966355aeb266514f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~RealtimeAsyncUpdater</name>
      <anchorfile>classRealtimeAsyncUpdater.html</anchorfile>
      <anchor>a8a04b37b535881acf0dbcd49399f274e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>triggerAsyncUpdate</name>
      <anchorfile>classRealtimeAsyncUpdater.html</anchorfile>
      <anchor>aee4e7bf67d31d0bb574132bfd102f57f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cancelPendingUpdate</name>
      <anchorfile>classRealtimeAsyncUpdater.html</anchorfile>
      <anchor>a925b9c4623b6c0c9652484e64677502b</anchor>
      <arglist>() noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>handleUpdateNowIfNeeded</name>
      <anchorfile>classRealtimeAsyncUpdater.html</anchorfile>
      <anchor>accae6ff15d6eff08e6a3318263eb6574</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isUpdatePending</name>
      <anchorfile>classRealtimeAsyncUpdater.html</anchorfile>
      <anchor>acd6cd91c301fd331bc4c48ec009adc5a</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual void</type>
      <name>handleAsyncUpdate</name>
      <anchorfile>classRealtimeAsyncUpdater.html</anchorfile>
      <anchor>a86dee41bf003fdbf125d0b3683acbaf0</anchor>
      <arglist>()=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ResamplingFifo</name>
    <filename>classResamplingFifo.html</filename>
    <member kind="function">
      <type></type>
      <name>ResamplingFifo</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>a2ee165634dd4f6e42f6611160799a279</anchor>
      <arglist>(int blockSize, int numChannels=2, int maxSamples=44100)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ResamplingFifo</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>ac7b1ac0c7f6c77bf7f4f5c1a2ac29240</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSize</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>ae90257c3c811af93e2a94cd8a993b3d2</anchor>
      <arglist>(int blockSize, int numChannels=2, int maxSamples=44100)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setResamplingRatio</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>ad7f9772deb55bfa96d54ac1d76c14670</anchor>
      <arglist>(double inputRate, double outputRate)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRatio</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>ad0e91a85aa9e96edc898420cf07c23e4</anchor>
      <arglist>(float ratio)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getRatio</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>a9e2c7ea1b975a735a00bcc3944b1b090</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setQuality</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>af4b41c9f4716e397d916e3329990f4d3</anchor>
      <arglist>(int q)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>ab8c13bc10787cc255b4d4bdc88f5ce34</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>samplesReady</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>a932efb27236ccc341c04bd56193e283e</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pushAudioBuffer</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>aac26c115b05721bfae78db814aaa22cd</anchor>
      <arglist>(const juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>popAudioBuffer</name>
      <anchorfile>classResamplingFifo.html</anchorfile>
      <anchor>a18fbc4f4369ffdddc6ceacbb95b7997e</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RollingAverage</name>
    <filename>classRollingAverage.html</filename>
    <member kind="function">
      <type></type>
      <name>RollingAverage</name>
      <anchorfile>classRollingAverage.html</anchorfile>
      <anchor>ac31bf3c8f185244cad3ec353a3156888</anchor>
      <arglist>(int numVals_)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>average</name>
      <anchorfile>classRollingAverage.html</anchorfile>
      <anchor>a941797e5f59cbdfce52faa655bd1b7f3</anchor>
      <arglist>(double nextValue)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getAverage</name>
      <anchorfile>classRollingAverage.html</anchorfile>
      <anchor>a6dd6f624ec61f187cbe4c46810fa03e4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setAverage</name>
      <anchorfile>classRollingAverage.html</anchorfile>
      <anchor>a0c890e31baee94c999c2e7086ece4469</anchor>
      <arglist>(double avg)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Sample</name>
    <filename>classSample.html</filename>
    <member kind="function">
      <type>int</type>
      <name>getLength</name>
      <anchorfile>classSample.html</anchorfile>
      <anchor>ac7a3b9f9b37ddd788e81056b34427573</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getRootNote</name>
      <anchorfile>classSample.html</anchorfile>
      <anchor>abe1e1e9fc398382c019ca15249cc13c4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>getSampleRate</name>
      <anchorfile>classSample.html</anchorfile>
      <anchor>a6c25a4367bb3f7e3ec986e96c2472015</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::AudioSampleBuffer &amp;</type>
      <name>getSamples</name>
      <anchorfile>classSample.html</anchorfile>
      <anchor>a179818f303e3ad2921683524798591f8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>read</name>
      <anchorfile>classSample.html</anchorfile>
      <anchor>afa908998646ca749fe3474d90f747df2</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, int samplePos, int bufferPos=0, int length=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>readSamplesAt</name>
      <anchorfile>classSample.html</anchorfile>
      <anchor>a0bece656bf856bb465ab23995a7dc608</anchor>
      <arglist>(int pos, float &amp;lOut, float &amp;rOut)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::unique_ptr&lt; Sample &gt;</type>
      <name>fromFile</name>
      <anchorfile>classSample.html</anchorfile>
      <anchor>a6ad98110e9ccc23c8ae9d9fe7f7914f9</anchor>
      <arglist>(const juce::File &amp;f)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SampleOscillator</name>
    <filename>classSampleOscillator.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Direction</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a64aa6cb05a5893d3a6e0220f61e6a6e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>forwards</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a64aa6cb05a5893d3a6e0220f61e6a6e1a465c5f747934848fce35cebef03936e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backwards</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a64aa6cb05a5893d3a6e0220f61e6a6e1a09ff5fc2d1c28a5c0a03e54e5d1a8e4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>pingpong</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a64aa6cb05a5893d3a6e0220f61e6a6e1adc80e6dce6b6fd92663fdd3c14ea4a6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>forwards</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a64aa6cb05a5893d3a6e0220f61e6a6e1a465c5f747934848fce35cebef03936e2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backwards</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a64aa6cb05a5893d3a6e0220f61e6a6e1a09ff5fc2d1c28a5c0a03e54e5d1a8e4a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>pingpong</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a64aa6cb05a5893d3a6e0220f61e6a6e1adc80e6dce6b6fd92663fdd3c14ea4a6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleOscillator</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a74f851f7bdfe25d92a5982162a9f8abc</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRateAndBlockSize</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a379a84c46a29392e94cc32d4a20b9a91</anchor>
      <arglist>(double sr, int bs)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSample</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a806bd14975ddf68765f98f96a0a8ed2c</anchor>
      <arglist>(Sample *)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRange</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a17333bd292081508797d6b32e1bce90e</anchor>
      <arglist>(int s, int e)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLooping</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a6ad97b31d76399ec7140ba7db4bf0526</anchor>
      <arglist>(bool l, int s=0, int e=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDirection</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>abf98cd2cb6802406945ba3fa505797a7</anchor>
      <arglist>(Direction d)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCrossfadeDuration</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>afdf6664481fa2ea9c77829deb7c4e1cd</anchor>
      <arglist>(double s)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a6e4b9ad2169c8acc82089e89ec8b1012</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOff</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>a0a91c401c4b160c05556e460097e348a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>read</name>
      <anchorfile>classSampleOscillator.html</anchorfile>
      <anchor>aa22fc5c8f2ec429eab3d42e6a6524923</anchor>
      <arglist>(double note, juce::AudioSampleBuffer &amp;buffer, int start=0, int length=-1)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ScratchBuffer</name>
    <filename>classScratchBuffer.html</filename>
    <member kind="function">
      <type></type>
      <name>ScratchBuffer</name>
      <anchorfile>classScratchBuffer.html</anchorfile>
      <anchor>a2a370ef469289ac0f993a0e48caa3239</anchor>
      <arglist>(int numChannels, int numSamples)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>ScratchBuffer</name>
      <anchorfile>classScratchBuffer.html</anchorfile>
      <anchor>a27bf06657e9335019dd4b815cbf7b789</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~ScratchBuffer</name>
      <anchorfile>classScratchBuffer.html</anchorfile>
      <anchor>ad1064185b708fea7cc96514eae47ecdd</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SecureStreamingSocket</name>
    <filename>classSecureStreamingSocket.html</filename>
    <member kind="function">
      <type></type>
      <name>SecureStreamingSocket</name>
      <anchorfile>classSecureStreamingSocket.html</anchorfile>
      <anchor>a0a4404801edc197e4fad99088c5aba73</anchor>
      <arglist>(bool secure=true)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~SecureStreamingSocket</name>
      <anchorfile>classSecureStreamingSocket.html</anchorfile>
      <anchor>a96789a7cbc6d6616b95480182ecea91a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>connect</name>
      <anchorfile>classSecureStreamingSocket.html</anchorfile>
      <anchor>aea1e67df0bbf6e3f7cd82e7b6419c1e8</anchor>
      <arglist>(const juce::String &amp;remoteHostname, int remotePortNumber, int timeOutMillisecs=3000)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>close</name>
      <anchorfile>classSecureStreamingSocket.html</anchorfile>
      <anchor>ae57da84b07292822018a7a3e1f3dd903</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isConnected</name>
      <anchorfile>classSecureStreamingSocket.html</anchorfile>
      <anchor>ac3b5f2ebb259e00af2aaaaa31b97cd77</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>read</name>
      <anchorfile>classSecureStreamingSocket.html</anchorfile>
      <anchor>a09004131d1a2795ff2f9583c0b596d83</anchor>
      <arglist>(void *destBuffer, int maxBytesToRead, bool blockUntilSpecifiedAmountHasArrived)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>write</name>
      <anchorfile>classSecureStreamingSocket.html</anchorfile>
      <anchor>ae04b0755561c2b33d447a43712c16c0f</anchor>
      <arglist>(const void *sourceBuffer, int numBytesToWrite)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getRawSocketHandle</name>
      <anchorfile>classSecureStreamingSocket.html</anchorfile>
      <anchor>a8d6d63b2c24572b03848be577bb1efb5</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>waitUntilReady</name>
      <anchorfile>classSecureStreamingSocket.html</anchorfile>
      <anchor>ae5b14633c70b66812ea2647905ef78f2</anchor>
      <arglist>(bool readyForReading, int timeoutMsecs)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Select</name>
    <filename>classSelect.html</filename>
    <base>ParamComponent</base>
    <member kind="function">
      <type></type>
      <name>Select</name>
      <anchorfile>classSelect.html</anchorfile>
      <anchor>ac3aa83683f719ac41bc4c4ac559faa59</anchor>
      <arglist>(Parameter *parameter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setShowName</name>
      <anchorfile>classSelect.html</anchorfile>
      <anchor>a4b9a602ca81e69b5ae3f681274694fc4</anchor>
      <arglist>(bool s)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>resized</name>
      <anchorfile>classSelect.html</anchorfile>
      <anchor>a3c244c1a02c2a16152ed0f562b23b928</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>parentHierarchyChanged</name>
      <anchorfile>classSelect.html</anchorfile>
      <anchor>ae1f2c2b54bf8dfd244857e18dfe22849</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::Label</type>
      <name>name</name>
      <anchorfile>classSelect.html</anchorfile>
      <anchor>a68477089c5c406d199fe071d09080be4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>PluginComboBox</type>
      <name>comboBox</name>
      <anchorfile>classSelect.html</anchorfile>
      <anchor>ae4c36d9f29cde76f2dadd791c8c3ed05</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>showName</name>
      <anchorfile>classSelect.html</anchorfile>
      <anchor>aef337a5f41361db7b4dac795bb32d1db</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SharedMemory</name>
    <filename>classSharedMemory.html</filename>
    <member kind="function">
      <type></type>
      <name>SharedMemory</name>
      <anchorfile>classSharedMemory.html</anchorfile>
      <anchor>a7ed6c8ab72fa4eaf0abd6d665df0e210</anchor>
      <arglist>(const juce::String &amp;name, int size)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~SharedMemory</name>
      <anchorfile>classSharedMemory.html</anchorfile>
      <anchor>adc56751ce1b30224689e71cd3b6c09be</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void *</type>
      <name>getData</name>
      <anchorfile>classSharedMemory.html</anchorfile>
      <anchor>a37bfb82706e0e10282fc0f899818f0d9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getSize</name>
      <anchorfile>classSharedMemory.html</anchorfile>
      <anchor>ac16dde725ff9ce10788b9ff805ba3e85</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static void</type>
      <name>remove</name>
      <anchorfile>classSharedMemory.html</anchorfile>
      <anchor>a19714c21ee5c98faff8a1e77e79532a7</anchor>
      <arglist>(const juce::String &amp;name)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SimpleVerb</name>
    <filename>classSimpleVerb.html</filename>
    <member kind="function">
      <type></type>
      <name>SimpleVerb</name>
      <anchorfile>classSimpleVerb.html</anchorfile>
      <anchor>a1792ace0ad36c1f9f280096c4d6a1fe1</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classSimpleVerb.html</anchorfile>
      <anchor>a3375884bdc9570f8d07f6f0575c90283</anchor>
      <arglist>(float sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classSimpleVerb.html</anchorfile>
      <anchor>a2c675ca200b1eab943bac840224fde97</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParameters</name>
      <anchorfile>classSimpleVerb.html</anchorfile>
      <anchor>a69ee1d22a4025b221a5af157bdeca96d</anchor>
      <arglist>(float roomIn, float dampIn, float preDelayIn, float lpFaderIn, float hpFaderIn, float wetIn, float dryIn)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SingleLineTextEditor</name>
    <filename>classSingleLineTextEditor.html</filename>
    <class kind="class">SingleLineTextEditor::InputFilter</class>
    <class kind="class">SingleLineTextEditor::LengthAndCharacterRestriction</class>
    <class kind="class">SingleLineTextEditor::Listener</class>
    <class kind="struct">SingleLineTextEditor::LookAndFeelMethods</class>
    <member kind="enumeration">
      <type></type>
      <name>ColourIds</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13caf8ba7094bc5e6cd79e7a007eafb8739b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>textColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13ca3139e08c59ac1e595c5029d4c90b3a95</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highlightColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13ca237763d04e9174925d2e0589d8cc8390</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highlightedTextColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13ca9b056fa03362c023ccfa253404c8893a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>outlineColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13caeddcf10206bd667dafb96511c583dfcd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>focusedOutlineColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13ca613b4466a2f85ed3a22af2012c77b720</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>shadowColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13cac438c89220c8873c78bee2606a59b1fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13caf8ba7094bc5e6cd79e7a007eafb8739b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>textColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13ca3139e08c59ac1e595c5029d4c90b3a95</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highlightColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13ca237763d04e9174925d2e0589d8cc8390</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>highlightedTextColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13ca9b056fa03362c023ccfa253404c8893a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>outlineColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13caeddcf10206bd667dafb96511c583dfcd</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>focusedOutlineColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13ca613b4466a2f85ed3a22af2012c77b720</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>shadowColourId</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a580e95f56d303528b02f91143c72f13cac438c89220c8873c78bee2606a59b1fe</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SingleLineTextEditor</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a2c7973e7c8af497c83b6a551f444e2dc</anchor>
      <arglist>(const juce::String &amp;componentName=juce::String())</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~SingleLineTextEditor</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a770c8d06c1e7c25ccc11ce11bad508f5</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTabKeyUsedAsCharacter</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a01ecd694f39ff602a312ed1222f383e9</anchor>
      <arglist>(bool shouldTabKeyBeUsed)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isTabKeyUsedAsCharacter</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a3eb6a547be90336c41564adea8ff0c5a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setEscapeAndReturnKeysConsumed</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a2421745b102e2acc7a889dcbd2738e5a</anchor>
      <arglist>(bool shouldBeConsumed) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setReadOnly</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a26bae8f731370b8287fc458de67792a2</anchor>
      <arglist>(bool shouldBeReadOnly)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isReadOnly</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aecab0d2e288a280d8c25c6d523df9635</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCaretVisible</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a81668b4dcb486a79e1aed0ae94e8f3e4</anchor>
      <arglist>(bool shouldBeVisible)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isCaretVisible</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a0b0f9a73a53caa43db1bd61aafee94a7</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPopupMenuEnabled</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a77cf6c3087a5bd0878d9124aa0db6117</anchor>
      <arglist>(bool menuEnabled)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isPopupMenuEnabled</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a0437359209f66941797733f40d15d0f6</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isPopupMenuCurrentlyActive</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a815bf273efea2fafca659e56124696bc</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFont</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a549c247fef1645ded8450ad61a69b722</anchor>
      <arglist>(const juce::Font &amp;newFont)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyFontToAllText</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a1f2703bd0ea7f77bf9b9ef6cb939b986</anchor>
      <arglist>(const juce::Font &amp;newFont)</arglist>
    </member>
    <member kind="function">
      <type>const juce::Font &amp;</type>
      <name>getFont</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>add6ab602dcfc5140dc835ed398c490e3</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setJustificationType</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aa91356ec3b83897b64aab87d70579bc6</anchor>
      <arglist>(juce::Justification just)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSelectAllWhenFocused</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ad5a4c1fdf55890300bf5c4c43f800060</anchor>
      <arglist>(bool shouldSelectAll)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTextToShowWhenEmpty</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>adf573da13743e43c4f62ec9a0d802afa</anchor>
      <arglist>(const juce::String &amp;text, juce::Colour colourToUse)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addListener</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a6a7ea7be3feb0110dfba243e5b0d33c3</anchor>
      <arglist>(SingleLineTextEditor::Listener *newListener)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>removeListener</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a02ba20e1e8a843b64ac398ecf924c39b</anchor>
      <arglist>(SingleLineTextEditor::Listener *listenerToRemove)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getText</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ab7c48b757a09f3687b8f2a60690e1d61</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getTextInRange</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>acccbcba4eb83012fa4592da4d5ee6d76</anchor>
      <arglist>(const juce::Range&lt; int &gt; &amp;textRange) const override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isEmpty</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a348819d1d31721f60cab4deedd423331</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setText</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a6f4f38207cdbffa7a1033e96969cc7a3</anchor>
      <arglist>(const juce::String &amp;newText, bool sendTextChangeMessage=true)</arglist>
    </member>
    <member kind="function">
      <type>juce::Value &amp;</type>
      <name>getTextValue</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a2dd654dd2d43fed5244624e009c6163d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>insertTextAtCaret</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aaf4946076bc4ece47fabe839a1556694</anchor>
      <arglist>(const juce::String &amp;textToInsert) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ae4c48d22effebd9071ea97ec20a9371d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cut</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a66b8f77a6719edd8e1a4997003986a36</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ac70d898376c76d91384c873a59c787e9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paste</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aa3b8c17f58f57098a9562e2ac1fc8f51</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getCaretPosition</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a023fea15cd3ab896d9132560b673aea1</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCaretPosition</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a2a7e634d21820819e9c1e76fffe89d18</anchor>
      <arglist>(int newIndex)</arglist>
    </member>
    <member kind="function">
      <type>juce::Rectangle&lt; int &gt;</type>
      <name>getCaretRectangleForCharIndex</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a58789ac6c8cde4b93ff33677b43f2307</anchor>
      <arglist>(int index) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setHighlightedRegion</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a347c56fbc82297286cca76984675e3ee</anchor>
      <arglist>(const juce::Range&lt; int &gt; &amp;newSelection) override</arglist>
    </member>
    <member kind="function">
      <type>juce::Range&lt; int &gt;</type>
      <name>getHighlightedRegion</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ad2cd12fb16ba8d5a154f64c273ce2530</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getHighlightedText</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>af52842ecb6980a38a9c7dcdf20317ffd</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getTextIndexAt</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>af00ababad07f4e206ba60b6e603b98dd</anchor>
      <arglist>(int x, int y)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getTotalNumChars</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a130774066858dbbf22505f3cb24a01b4</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getTextWidth</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a0e543e4f447cf374ae8caecb9aff7166</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getTextHeight</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a13916c0b6d2325155ff68b6b1576f4b8</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>moveCaretToEnd</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aa3c49bcd8ddf2953996ef293b34395d4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>moveCaretLeft</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a1d1097512ddba6438a269cfa8b1a3ca8</anchor>
      <arglist>(bool moveInWholeWordSteps, bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>moveCaretRight</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a1952250385f355d97c2e8e3d76427119</anchor>
      <arglist>(bool moveInWholeWordSteps, bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>moveCaretUp</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a85164bf3e0f9ed386b1d4c4d8e510924</anchor>
      <arglist>(bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>moveCaretDown</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a805c5628029f6dfebe6173426a653ce4</anchor>
      <arglist>(bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>pageUp</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ac4bdcc6cbb27b1404fe45f5e78c6415d</anchor>
      <arglist>(bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>pageDown</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aade9b576ce0ddd06ed0cca96fc762da5</anchor>
      <arglist>(bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scrollDown</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ae2a391e5825f89d4340213bebf515ae6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>scrollUp</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ad315bd9802d7b9508669093601e496d6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>moveCaretToTop</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a7e17b365435d9689750f74c370da61fd</anchor>
      <arglist>(bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>moveCaretToStartOfLine</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aed7187d7071b851adc5bd93c97e77f80</anchor>
      <arglist>(bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>moveCaretToEnd</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a6637b79424cf07d34ced878d17212a7b</anchor>
      <arglist>(bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>moveCaretToEndOfLine</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a15d821013aa5a9b55bcf8bb4877bee62</anchor>
      <arglist>(bool selecting)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>deleteBackwards</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a375af0176e428c9c8213929047111b76</anchor>
      <arglist>(bool moveInWholeWordSteps)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>deleteForwards</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a696359b4ec5f6a7573888294d2db1dfd</anchor>
      <arglist>(bool moveInWholeWordSteps)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>copyToClipboard</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a2f481549128e342b2cfae10d7c1f3e34</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>cutToClipboard</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a453715d391f524eb766adb5915418954</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>pasteFromClipboard</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a2978e8cc1b0056647096e9f95d3a6d0a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>selectAll</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ac0cdd47c751c6076bea25d95c89b05dd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>undo</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aa7dde1f6c14d55b60a7a3804983edbdd</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>redo</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a2e5d68158701ea32686a11e39d1f7126</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>addPopupMenuItems</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>acedbc6d806838a38cbf9910cd6050a32</anchor>
      <arglist>(juce::PopupMenu &amp;menuToAddTo, const juce::MouseEvent *mouseClickEvent)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>performPopupMenuAction</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a96db2ae0741c6c2c5cd7cf5fae13e9fe</anchor>
      <arglist>(int menuItemID)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setInputFilter</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a5b80bbc1b2609ead0eb54cf0b2f57563</anchor>
      <arglist>(InputFilter *newFilter, bool takeOwnership)</arglist>
    </member>
    <member kind="function">
      <type>InputFilter *</type>
      <name>getInputFilter</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a1450af86445075d23859084a359b4222</anchor>
      <arglist>() const noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setInputRestrictions</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ab071d4d2aeb8e84668de465b6dcc2033</anchor>
      <arglist>(int maxTextLength, const juce::String &amp;allowedCharacters=juce::String())</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setKeyboardType</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ad0f5fa9475c74f3dd7d196089ce4a97c</anchor>
      <arglist>(VirtualKeyboardType type) noexcept</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ae140b5a9e5fcdf3e31bfef5940a053cd</anchor>
      <arglist>(juce::Graphics &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paintOverChildren</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a61b081f408caed8985fc3369b89d4d4d</anchor>
      <arglist>(juce::Graphics &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mouseDown</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aa2e13a5693aa6599a29393f76aeca434</anchor>
      <arglist>(const juce::MouseEvent &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mouseUp</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a202c0e9fd1c5447227a3132e9616a92e</anchor>
      <arglist>(const juce::MouseEvent &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mouseDrag</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a7632816fd86e6f3a83d9f746e5c9d59d</anchor>
      <arglist>(const juce::MouseEvent &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mouseDoubleClick</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>acb833ecaee0c50b4df48e2d337b27bdc</anchor>
      <arglist>(const juce::MouseEvent &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mouseWheelMove</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a6110f9ada85752b9c1d148235a31bcbc</anchor>
      <arglist>(const juce::MouseEvent &amp;, const juce::MouseWheelDetails &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>keyPressed</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a8dfacbc90d80e4f3989611c2ab14e616</anchor>
      <arglist>(const juce::KeyPress &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>keyStateChanged</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aaa6bb606d8c47ae0685b0dda0b3d0a35</anchor>
      <arglist>(bool) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>focusGained</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a2637496e94b565815585f09871e2cda2</anchor>
      <arglist>(FocusChangeType) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>focusLost</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a064d28e1648885e900dd9a0f9da3701a</anchor>
      <arglist>(FocusChangeType) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resized</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ae21f117bacc258d8c67c04d014846cad</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>enablementChanged</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ab5cf90068efe93555678f60f46d934e4</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>colourChanged</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aa399bbb7a5f8b4e80d39b66700ca720c</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lookAndFeelChanged</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ac57e64a781fa1a545bb4fd14116828bb</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isTextInputActive</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a59f2ceba58a9eda7cd57fbe45a7aae02</anchor>
      <arglist>() const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTemporaryUnderlining</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>aceb15841e3da110210b97d27f7121856</anchor>
      <arglist>(const juce::Array&lt; juce::Range&lt; int &gt; &gt; &amp;) override</arglist>
    </member>
    <member kind="function">
      <type>VirtualKeyboardType</type>
      <name>getKeyboardType</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ab59702bb98c360dfd0056695c42b0b58</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>textChanged</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a7432325931d3ad2a10f0b98fb83f8ec7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>newTransaction</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>ab5aafa23521231a8b7f6811b2d937b44</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>returnPressed</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a6ea46ada81c1fc1acfd818740832a9d6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected" virtualness="virtual">
      <type>virtual void</type>
      <name>escapePressed</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a68de7f5755634511183f84ebebab29a7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="friend" protection="private">
      <type>friend class</type>
      <name>InsertAction</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>abc5356010799f2128077c55e9617ff8b</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend" protection="private">
      <type>friend class</type>
      <name>RemoveAction</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a62a43dc3d2ac160c038ec212549e15c1</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend" protection="private">
      <type>friend class</type>
      <name>TextHolderComponent</name>
      <anchorfile>classSingleLineTextEditor.html</anchorfile>
      <anchor>a8d20cf4fd62ae3c46f37b9bfc1487a9a</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SmoothedParameter</name>
    <filename>classSmoothedParameter.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type></type>
      <name>SmoothedParameter</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>afc3e5428bf61be1df757d73102703a0e</anchor>
      <arglist>(Processor &amp;p, juce::String uid_, juce::String name_, juce::String shortName_, juce::String label_, float minValue_, float maxValue_, float intervalValue_, float defaultValue_, float skewFactor_=1.0f, std::function&lt; juce::String(const gin::Parameter &amp;, float)&gt; textFunction_=nullptr)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SmoothedParameter</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>ab8311605dca336ed94b4f0497a2649fb</anchor>
      <arglist>(Processor &amp;p, juce::String uid_, juce::String name_, juce::String shortName_, juce::String label_, juce::NormalisableRange&lt; float &gt; range_, float defaultValue_, std::function&lt; juce::String(const gin::Parameter &amp;, float)&gt; textFunction_=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>prepareToPlay</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>aa7fcd3012e8f526fa9ba08117744c4b7</anchor>
      <arglist>(double sampleRate, int) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>a2426967e4e401180c3740087fee1e69e</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setUserValue</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>a5fc33dae16fc8a344df404d4794c1d61</anchor>
      <arglist>(float v) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setUserValueNotifingHost</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>aff236e911f028b816804ce22a4d15710</anchor>
      <arglist>(float v) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>ac739a04f2772dbbfbe2293f8793fa7e1</anchor>
      <arglist>(float valueIn) override</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getProcValue</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>a0d368f6b4aa20b3a5adb60b17a70d011</anchor>
      <arglist>(int stepSize) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSmoothingTime</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>af89156417f953ca71f34b50bdc65021e</anchor>
      <arglist>(float t)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isSmoothingActive</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>a30b054712a01a8376e8071926c0002a3</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>T</type>
      <name>smoother</name>
      <anchorfile>classSmoothedParameter.html</anchorfile>
      <anchor>a21d20d9d589e79580860eb8d21b7e53e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SmoothingType</name>
    <filename>classSmoothingType.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Type</name>
      <anchorfile>classSmoothingType.html</anchorfile>
      <anchor>a3d81776be90ddd188fa74ae1b80fff7c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>linear</name>
      <anchorfile>classSmoothingType.html</anchorfile>
      <anchor>a3d81776be90ddd188fa74ae1b80fff7ca6f9f428f8d2b02f3abdd34b9c11f1f84</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eased</name>
      <anchorfile>classSmoothingType.html</anchorfile>
      <anchor>a3d81776be90ddd188fa74ae1b80fff7ca9ba4cec564f640dc7fef74aafe60e3df</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>linear</name>
      <anchorfile>classSmoothingType.html</anchorfile>
      <anchor>a3d81776be90ddd188fa74ae1b80fff7ca6f9f428f8d2b02f3abdd34b9c11f1f84</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>eased</name>
      <anchorfile>classSmoothingType.html</anchorfile>
      <anchor>a3d81776be90ddd188fa74ae1b80fff7ca9ba4cec564f640dc7fef74aafe60e3df</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SmoothingType</name>
      <anchorfile>classSmoothingType.html</anchorfile>
      <anchor>a9db5125701b5cafeee63f81f7550d320</anchor>
      <arglist>(float time_=0.1f, Type type_=linear)</arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>time</name>
      <anchorfile>classSmoothingType.html</anchorfile>
      <anchor>afa303f9e26f8612da8c4d27f9c0ba87a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Type</type>
      <name>type</name>
      <anchorfile>classSmoothingType.html</anchorfile>
      <anchor>a0393375a369c88015c76fe37f4e50a1c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Spline</name>
    <filename>classSpline.html</filename>
    <class kind="class">Spline::Element</class>
    <member kind="function">
      <type></type>
      <name>Spline</name>
      <anchorfile>classSpline.html</anchorfile>
      <anchor>adc45d0c48af983c27e7657ce0ad5bc72</anchor>
      <arglist>(const juce::Array&lt; Point&lt; double &gt;&gt; &amp;points)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator[]</name>
      <anchorfile>classSpline.html</anchorfile>
      <anchor>a30255a2f77480c04928394262765df10</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>interpolate</name>
      <anchorfile>classSpline.html</anchorfile>
      <anchor>ac1f059b0c61e23b88d7939a27fd40b5c</anchor>
      <arglist>(double x) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>StepLFO</name>
    <filename>classStepLFO.html</filename>
    <member kind="function">
      <type></type>
      <name>StepLFO</name>
      <anchorfile>classStepLFO.html</anchorfile>
      <anchor>a3868a6372c5e5ea5c024815bda225197</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classStepLFO.html</anchorfile>
      <anchor>a3e1fa83dba7591409abbbabf92fca91b</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFreq</name>
      <anchorfile>classStepLFO.html</anchorfile>
      <anchor>aa4eb2d01feb657be92b0b1b652ccdb13</anchor>
      <arglist>(float f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setNumPoints</name>
      <anchorfile>classStepLFO.html</anchorfile>
      <anchor>a1e44b3d5beb8294351f85e63c830e3c1</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPoint</name>
      <anchorfile>classStepLFO.html</anchorfile>
      <anchor>ae703cc83910512030357a78a65d532aa</anchor>
      <arglist>(int n, float v)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classStepLFO.html</anchorfile>
      <anchor>aed045806e2df53ed176f8a7e109d5283</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classStepLFO.html</anchorfile>
      <anchor>a6eed3f2df15ee299ecb928c8bff961e2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>process</name>
      <anchorfile>classStepLFO.html</anchorfile>
      <anchor>a7a85cf66615eda334a7ca3a71c3903b3</anchor>
      <arglist>(int numSamples)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getOutput</name>
      <anchorfile>classStepLFO.html</anchorfile>
      <anchor>abfcd3d3a5e7d82c4f30da1534e8c39bc</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>StepLFOComponent</name>
    <filename>classStepLFOComponent.html</filename>
    <base>MultiParamComponent</base>
    <member kind="function">
      <type></type>
      <name>StepLFOComponent</name>
      <anchorfile>classStepLFOComponent.html</anchorfile>
      <anchor>a21e66ada757a7e91e01c9314c3c5cbd0</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~StepLFOComponent</name>
      <anchorfile>classStepLFOComponent.html</anchorfile>
      <anchor>a9dce47acb1fa36c16921da9f415914bf</anchor>
      <arglist>() override=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setBPM</name>
      <anchorfile>classStepLFOComponent.html</anchorfile>
      <anchor>a7a29b7358ef0aeb985255dc51818855a</anchor>
      <arglist>(float bpm)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classStepLFOComponent.html</anchorfile>
      <anchor>afa1dbeb6c10d91bd32ec8174efe1f388</anchor>
      <arglist>(Parameter::Ptr beat, Parameter::Ptr length, Parameter::Ptr *level, Parameter::Ptr enable)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>StereoDelay</name>
    <filename>classStereoDelay.html</filename>
    <member kind="function">
      <type></type>
      <name>StereoDelay</name>
      <anchorfile>classStereoDelay.html</anchorfile>
      <anchor>a3489a6b571a1083bb4b4ff8621e10993</anchor>
      <arglist>(double maxDelay_=5.1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classStereoDelay.html</anchorfile>
      <anchor>ad0a006720ea08d0bc0e9da0b11deeccb</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classStereoDelay.html</anchorfile>
      <anchor>a28569f59c5a91b46001bc64da62ca04b</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParams</name>
      <anchorfile>classStereoDelay.html</anchorfile>
      <anchor>aa587ccb3ad71e93752569cb25803f0a4</anchor>
      <arglist>(float delay_, float mix_, float feedback_, float crossfeed_)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classStereoDelay.html</anchorfile>
      <anchor>aa3d97ad1b7370dc061d9f73daa574c2b</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>StereoEnhancer</name>
    <filename>classStereoEnhancer.html</filename>
    <member kind="function">
      <type></type>
      <name>StereoEnhancer</name>
      <anchorfile>classStereoEnhancer.html</anchorfile>
      <anchor>a5e466244692e1f4470a6950b2c55fdc2</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classStereoEnhancer.html</anchorfile>
      <anchor>a7f6872a6e01a7de9c680bc65ba32ef64</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classStereoEnhancer.html</anchorfile>
      <anchor>a4268cc461e66896416b0c266d66e127e</anchor>
      <arglist>(float sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParameters</name>
      <anchorfile>classStereoEnhancer.html</anchorfile>
      <anchor>a8aca4b6272efd156986e5254310b60ff</anchor>
      <arglist>(float widthLP, float freqHPFader, float widthHP)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>StereoOscillator</name>
    <filename>classStereoOscillator.html</filename>
    <class kind="struct">StereoOscillator::Params</class>
    <member kind="function">
      <type></type>
      <name>StereoOscillator</name>
      <anchorfile>classStereoOscillator.html</anchorfile>
      <anchor>abfa50f6bb465a3d7a29fac5e0bdbea68</anchor>
      <arglist>(BandLimitedLookupTables &amp;bllt_)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classStereoOscillator.html</anchorfile>
      <anchor>a720c29b79225736abece6bd93457a647</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classStereoOscillator.html</anchorfile>
      <anchor>aa553362f8f2510531675cdab875efe41</anchor>
      <arglist>(float p=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classStereoOscillator.html</anchorfile>
      <anchor>abdc5a2f2c152dd013c57fec8ed244ed1</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classStereoOscillator.html</anchorfile>
      <anchor>a17f31f43ebe17b636b493e704a7a599e</anchor>
      <arglist>(float noteL, float noteR, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processAdding</name>
      <anchorfile>classStereoOscillator.html</anchorfile>
      <anchor>a228048d564aa720b8a0aee6a624c089c</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processAdding</name>
      <anchorfile>classStereoOscillator.html</anchorfile>
      <anchor>a97fc34ee04e2aa2f488e761c257a19d9</anchor>
      <arglist>(float noteL, float noteR, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>StereoProcessor</name>
    <filename>classStereoProcessor.html</filename>
    <member kind="function">
      <type></type>
      <name>StereoProcessor</name>
      <anchorfile>classStereoProcessor.html</anchorfile>
      <anchor>adbfadef5bf37a546639db9a52caac2f3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classStereoProcessor.html</anchorfile>
      <anchor>a3ccdb1879a7ffd727fc88644de07ae72</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setParameters</name>
      <anchorfile>classStereoProcessor.html</anchorfile>
      <anchor>a847c55a3271b3cfb35c8d746fb7db19b</anchor>
      <arglist>(float width1, float centerFader1, float pan1, float rotationFader, float pan2, float centerFader2, float width2, float outVolFader)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Switch</name>
    <filename>classSwitch.html</filename>
    <base>ParamComponent</base>
    <member kind="function">
      <type></type>
      <name>Switch</name>
      <anchorfile>classSwitch.html</anchorfile>
      <anchor>ab2b13d51b74f2d3421a250e933cf697c</anchor>
      <arglist>(Parameter *parameter)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Synthesiser</name>
    <filename>classSynthesiser.html</filename>
    <member kind="function">
      <type></type>
      <name>Synthesiser</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a8c313d6f1fa875790e5c79dc621ee437</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMono</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a2da6967991a1afc0cb0efca3d1ba92f7</anchor>
      <arglist>(bool m)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setNumVoices</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a554b5cf0af6b3a40cdf60462ca34f425</anchor>
      <arglist>(int v)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLegato</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a1f66d6c7ec9c904dc22adca0d1df5c85</anchor>
      <arglist>(bool l)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setGlissando</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a208354c73e07365bdb9b429aae2457d9</anchor>
      <arglist>(bool g)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPortamento</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>ab6a82cda3e3f528112d665758efc21a3</anchor>
      <arglist>(bool p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setGlideRate</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a2e02ee6961b18517d08c8f249c02069e</anchor>
      <arglist>(float r)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMPE</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a7f8f310291795baffdc5ba124dc17adf</anchor>
      <arglist>(bool newMPE)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteAdded</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a29393cd7d99facd48ceab27dfa4ec3ba</anchor>
      <arglist>(juce::MPENote newNote) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteReleased</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>af2d49c835d5a6e4ae8de4229350e93ca</anchor>
      <arglist>(juce::MPENote finishedNote) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteAddedMonoGlide</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>ad3ae115ddf8dd12ca549d7d7e3811200</anchor>
      <arglist>(juce::MPENote newNote)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteReleasedMonoGlide</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>aaecfce290ac3ee8527d376c348750fca</anchor>
      <arglist>(juce::MPENote finishedNote)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteAddedMono</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a6f6e4029de04659076c79e96c1089c1d</anchor>
      <arglist>(juce::MPENote newNote)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteReleasedMono</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a87d9afd3a12f884569c312b4d00d9744</anchor>
      <arglist>(juce::MPENote finishedNote)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isNotePlaying</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a3b97e8ca28654f0770963796eeab6d22</anchor>
      <arglist>(juce::MPENote &amp;n)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumActiveVoices</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a281b865ea89afe1dab3fda4b266e5b66</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::Array&lt; SynthesiserVoice * &gt;</type>
      <name>getActiveVoices</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a7913b52c1fd0d2e15571451f638b7d01</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getCpuUsage</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a1e0aa77487d67b3f87543a999007bf1c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>startBlock</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a3dc68b923888fa1d7785817b327bde6f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>endBlock</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>ab07522f14e94e187a83e7c0b0eca2ffb</anchor>
      <arglist>(int blockSize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>retriggerVoice</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>afb116548ea250273d01234050957cd2d</anchor>
      <arglist>(SynthesiserVoice *v, juce::MPENote note)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>startVoice</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a9c7f19726ca95a0d6609370efb70dfac</anchor>
      <arglist>(juce::MPESynthesiserVoice *v, juce::MPENote note)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateGlide</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a4bf9918f18a2c232db12a913fda743f4</anchor>
      <arglist>(juce::MPESynthesiserVoice *v, juce::MPENote note)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stopVoiceFastKill</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a6cad92043ec0628345ae62b8341a4cbe</anchor>
      <arglist>(juce::MPESynthesiserVoice *v, juce::MPENote note, bool tailOff)</arglist>
    </member>
    <member kind="function">
      <type>juce::MPESynthesiserVoice *</type>
      <name>findVoiceToSteal</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a40b2a5153b414eafba711837b4cf3124</anchor>
      <arglist>(juce::MPENote noteToStealVoiceFor=juce::MPENote()) const override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>renderNextBlock</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a04c618669f417fb370f7d3866da5f141</anchor>
      <arglist>(juce::AudioBuffer&lt; floatType &gt; &amp;outputAudio, const juce::MidiBuffer &amp;inputMidi, int startSample, int numSamples)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>noteOnIndex</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>aa5ba2f02b20bcad66e99e471aa934793</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>noteOffIndex</name>
      <anchorfile>classSynthesiser.html</anchorfile>
      <anchor>a11111b8660140c3d67efbd56cf1089fc</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SynthesiserUsage</name>
    <filename>classSynthesiserUsage.html</filename>
    <member kind="function">
      <type></type>
      <name>SynthesiserUsage</name>
      <anchorfile>classSynthesiserUsage.html</anchorfile>
      <anchor>ae37e6d848cf06acd4d3795c7d86c62cd</anchor>
      <arglist>(Synthesiser &amp;s)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>timerCallback</name>
      <anchorfile>classSynthesiserUsage.html</anchorfile>
      <anchor>a61174588450de0af35bf0d1a3866faba</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classSynthesiserUsage.html</anchorfile>
      <anchor>a821a190f2151fc3a6022748679d283c0</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="variable">
      <type>Synthesiser &amp;</type>
      <name>synthesiser</name>
      <anchorfile>classSynthesiserUsage.html</anchorfile>
      <anchor>aab0e60a078f566eefd3c2de30e987486</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>voices</name>
      <anchorfile>classSynthesiserUsage.html</anchorfile>
      <anchor>a2cb8e573343dc51b45899177cc111574</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>cpu</name>
      <anchorfile>classSynthesiserUsage.html</anchorfile>
      <anchor>a10170c9eadde302226e4d25c1586d76e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::Path</type>
      <name>voicePath</name>
      <anchorfile>classSynthesiserUsage.html</anchorfile>
      <anchor>ac45af775287b4546013be488a67d5d7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>juce::Path</type>
      <name>cpuPath</name>
      <anchorfile>classSynthesiserUsage.html</anchorfile>
      <anchor>ac81b9aec980658fbc4bea45175e50ea5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SynthesiserVoice</name>
    <filename>classSynthesiserVoice.html</filename>
    <member kind="function">
      <type>void</type>
      <name>setFastKill</name>
      <anchorfile>classSynthesiserVoice.html</anchorfile>
      <anchor>a3ab87ba3f7df0e3b0aa91d56f0ce447f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isFastKill</name>
      <anchorfile>classSynthesiserVoice.html</anchorfile>
      <anchor>a4e34af2e0d789a4302de6de08052eb38</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>noteRetriggered</name>
      <anchorfile>classSynthesiserVoice.html</anchorfile>
      <anchor>a3d50ddac2700fd39a6a00f865de211ad</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCurrentlyPlayingNote</name>
      <anchorfile>classSynthesiserVoice.html</anchorfile>
      <anchor>af0b2e292d6a4cb5b54a131996a816978</anchor>
      <arglist>(juce::MPENote note)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setGlideInfo</name>
      <anchorfile>classSynthesiserVoice.html</anchorfile>
      <anchor>a414a524e658dda82521ee4279d337fe6</anchor>
      <arglist>(const GlideInfo &amp;gi)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>GlideInfo</type>
      <name>glideInfo</name>
      <anchorfile>classSynthesiserVoice.html</anchorfile>
      <anchor>a0b618c0309abdbff794b2c082dbaa993</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>fastKill</name>
      <anchorfile>classSynthesiserVoice.html</anchorfile>
      <anchor>aee3b10413c4c0203d9e8f26bb97cefaa</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SystemSemaphore</name>
    <filename>classSystemSemaphore.html</filename>
    <member kind="function">
      <type></type>
      <name>SystemSemaphore</name>
      <anchorfile>classSystemSemaphore.html</anchorfile>
      <anchor>ae7d9c999757639af86a49f838f862298</anchor>
      <arglist>(const juce::String &amp;name)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~SystemSemaphore</name>
      <anchorfile>classSystemSemaphore.html</anchorfile>
      <anchor>afc91d7c2c3b23850090654cd04d5a007</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>lock</name>
      <anchorfile>classSystemSemaphore.html</anchorfile>
      <anchor>a732fd5e497ace0769fce68aa9fe53764</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>unlock</name>
      <anchorfile>classSystemSemaphore.html</anchorfile>
      <anchor>afde687b04d0bb01cf629d037bf384ac0</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isValid</name>
      <anchorfile>classSystemSemaphore.html</anchorfile>
      <anchor>a03b3007a94f4d9ef3c735fda354c95c6</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>PatchBrowser::TagsModel</name>
    <filename>structPatchBrowser_1_1TagsModel.html</filename>
    <member kind="function">
      <type></type>
      <name>TagsModel</name>
      <anchorfile>structPatchBrowser_1_1TagsModel.html</anchorfile>
      <anchor>aa09938757cd8d6bf62458de6297aa5cb</anchor>
      <arglist>(PatchBrowser &amp;o)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>getNumRows</name>
      <anchorfile>structPatchBrowser_1_1TagsModel.html</anchorfile>
      <anchor>a030fdf75922a8dbe6f420f9323f94072</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>selectedRowsChanged</name>
      <anchorfile>structPatchBrowser_1_1TagsModel.html</anchorfile>
      <anchor>a4b8c665d5acbe7e4810a930f18d4923e</anchor>
      <arglist>(int) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paintListBoxItem</name>
      <anchorfile>structPatchBrowser_1_1TagsModel.html</anchorfile>
      <anchor>a6886ee4878b6e1127523ad086cc4ad45</anchor>
      <arglist>(int row, juce::Graphics &amp;g, int w, int h, bool selected) override</arglist>
    </member>
    <member kind="variable">
      <type>PatchBrowser &amp;</type>
      <name>owner</name>
      <anchorfile>structPatchBrowser_1_1TagsModel.html</anchorfile>
      <anchor>aba7ff3bb16cf0fb0f43d0841a4ecb6ca</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TextRenderer</name>
    <filename>classTextRenderer.html</filename>
    <member kind="function" static="yes">
      <type>static juce::String</type>
      <name>renderBuffer</name>
      <anchorfile>classTextRenderer.html</anchorfile>
      <anchor>a5d5364bd1d65cf1a8b5edd3a89a4ca14</anchor>
      <arglist>(const juce::AudioSampleBuffer &amp;buffer, int height, float samplesPerChar)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TimeProfiler</name>
    <filename>classTimeProfiler.html</filename>
    <member kind="function">
      <type></type>
      <name>TimeProfiler</name>
      <anchorfile>classTimeProfiler.html</anchorfile>
      <anchor>a7e6d1567e527185629c6c8aa46fe4943</anchor>
      <arglist>(const juce::String &amp;name_)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~TimeProfiler</name>
      <anchorfile>classTimeProfiler.html</anchorfile>
      <anchor>ab5a75d5cf6776bc1fe98074bc6ad9e8d</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TitleBar</name>
    <filename>classTitleBar.html</filename>
    <member kind="function">
      <type></type>
      <name>TitleBar</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>ae50c6223c4dd1df9d1bc5ad4864c271b</anchor>
      <arglist>(ProcessorEditor &amp;, Processor &amp;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~TitleBar</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>adf35e9e0331d6907b86b88a2abc70648</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>refreshPrograms</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a9dd5f1580e29745d8b2c61c569adfafb</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setShowBrowser</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a455717b90e823faab20135a3e8217750</anchor>
      <arglist>(bool)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setShowPresets</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>addc556044a2d728b9586ab3ac2e0abe7</anchor>
      <arglist>(bool)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a6d04cb46f66883940d2042c78f1d38eb</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>resized</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>aec8389eef90093162451137b8060a775</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>showMenu</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a9379495c07215f6e7ae0a8ef4fd3a0e9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>buttonClicked</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>af8e7c477535854c933acf07bd7e3fc7c</anchor>
      <arglist>(juce::Button *b) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>comboBoxChanged</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a9156b55473df7afcd442ab045a865cb3</anchor>
      <arglist>(juce::ComboBox *c) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>changeListenerCallback</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a33bfbb4b2223e0401bf883fbb8403991</anchor>
      <arglist>(juce::ChangeBroadcaster *) override</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>void</type>
      <name>parentHierarchyChanged</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>ab07f194ec9193585963f53a44eb24d72</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>ProcessorEditor &amp;</type>
      <name>editor</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a69203b2948adc0bf2c9841bd9511a3e6</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>Processor &amp;</type>
      <name>slProc</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>afd45058b0880e55b0da0aebf57b1f286</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>hasPresets</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a981c2205d4edca7359eafa06aa34931a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>bool</type>
      <name>hasBrowser</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a6782ccbfb8fc433a7abba03171506a8a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::ComboBox</type>
      <name>programs</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>aadc7b06a48172e5cc2a50d09a433fa77</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::TextButton</type>
      <name>addButton</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>aaa209338de7ffceada8ea42c3e04388b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::TextButton</type>
      <name>deleteButton</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a8b06f5f09bb781f1fd995fbb779891a3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::TextButton</type>
      <name>browseButton</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a48362c20ca1502a0a2bea5052faec7db</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::TextButton</type>
      <name>nextButton</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>ac438784aad0f7f333a135f3cf438413e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::TextButton</type>
      <name>prevButton</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>adf10f987b0a483ae72a005ed88e8dbc7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::TextButton</type>
      <name>menuButton</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a1be32416c074def83f54a9c1beefbdeb</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::TextButton</type>
      <name>infoButton</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a8677063fe0a948ce107b84ba444f46d2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; NewsChecker &gt;</type>
      <name>newsChecker</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>ab00ac1e8d688d411c19dc7ee9e3feac7</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>std::unique_ptr&lt; UpdateChecker &gt;</type>
      <name>updateChecker</name>
      <anchorfile>classTitleBar.html</anchorfile>
      <anchor>a592d0a4299aa23c9b323205d76f19742</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TriggeredScope</name>
    <filename>classTriggeredScope.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ColourIds</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ac8e18f9a32051394436ce1959334ac52</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ac8e18f9a32051394436ce1959334ac52ad98484fb436778a1891eb5b93e9c369e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ac8e18f9a32051394436ce1959334ac52abf8044b50950cbbf3641b221473b3f58</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>traceColourId</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ac8e18f9a32051394436ce1959334ac52a589de8d7a323fdf9be76f7ad5df7c2c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>envelopeColourId</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ac8e18f9a32051394436ce1959334ac52a0718ef5849150c461c04efe4d47d41a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumeration">
      <type></type>
      <name>TriggerMode</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a4a36f94df329bf1351904f51ff1aac40</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>None</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a4a36f94df329bf1351904f51ff1aac40ae7f5e1acdd8d7e12e9a4e84ada96839e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Up</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a4a36f94df329bf1351904f51ff1aac40a7f9a9c97a4c7f8e2424ec0827d879c71</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Down</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a4a36f94df329bf1351904f51ff1aac40a928f3c3101863b6dbc9c6d8fcb9ba9d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ac8e18f9a32051394436ce1959334ac52ad98484fb436778a1891eb5b93e9c369e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ac8e18f9a32051394436ce1959334ac52abf8044b50950cbbf3641b221473b3f58</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>traceColourId</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ac8e18f9a32051394436ce1959334ac52a589de8d7a323fdf9be76f7ad5df7c2c5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>envelopeColourId</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ac8e18f9a32051394436ce1959334ac52a0718ef5849150c461c04efe4d47d41a8</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>None</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a4a36f94df329bf1351904f51ff1aac40ae7f5e1acdd8d7e12e9a4e84ada96839e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Up</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a4a36f94df329bf1351904f51ff1aac40a7f9a9c97a4c7f8e2424ec0827d879c71</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Down</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a4a36f94df329bf1351904f51ff1aac40a928f3c3101863b6dbc9c6d8fcb9ba9d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>TriggeredScope</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a8ed01ea728d7ee2800bcfe0caef7516b</anchor>
      <arglist>(AudioFifo &amp;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~TriggeredScope</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a721552b49d9c8dbc2522d2b4221ad997</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setNumChannels</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>abcb75a1d83a267bbc2d631933013c8b3</anchor>
      <arglist>(int num)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setNumSamplesPerPixel</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>abb3a5feb7ff5f4bf2fc867ec0d70b025</anchor>
      <arglist>(float newNumSamplesPerPixel)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setVerticalZoomFactor</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a1ae62f1a0a576fe7a1eacf4a9625ca08</anchor>
      <arglist>(float newVerticalZoomFactor)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setVerticalZoomOffset</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a6cdb12229a253a243c18f72c92470a5b</anchor>
      <arglist>(float newVerticalZoomOffset, int ch)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSingleTrigger</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>afc087a187ae41f73400438e07c172c0d</anchor>
      <arglist>(bool singleTrigger_)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resetTrigger</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a591d6ef5a24faafa899825294c6c77a5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTriggerMode</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>aaae16520ddd08161c2242bd061df640f</anchor>
      <arglist>(TriggerMode newTriggerMode)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTriggerChannel</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a04d0e843acdc4b1455000a2d4225808c</anchor>
      <arglist>(int ch)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTriggerLevel</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a016127ffb60bb7b35eec997f1749b43a</anchor>
      <arglist>(float l)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTriggerPos</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>a28b7ad8c19d534150667d3e9cafe1f37</anchor>
      <arglist>(float l)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setDrawTriggerPos</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ad268f7c432368a7dd0721fd6f5889b3a</anchor>
      <arglist>(bool d)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ab4ea2e794f499a46a73adad7ed66c46f</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>timerCallback</name>
      <anchorfile>classTriggeredScope.html</anchorfile>
      <anchor>ae048c8050cd95f65d336801b1bf341e2</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>UpdateChecker</name>
    <filename>classUpdateChecker.html</filename>
    <member kind="function">
      <type></type>
      <name>UpdateChecker</name>
      <anchorfile>classUpdateChecker.html</anchorfile>
      <anchor>a991c35a0e515fa13cb9186936a00cb8f</anchor>
      <arglist>(Processor &amp;slProc)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~UpdateChecker</name>
      <anchorfile>classUpdateChecker.html</anchorfile>
      <anchor>aafcf15d0614046579e621995b19df9bd</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getUpdateUrl</name>
      <anchorfile>classUpdateChecker.html</anchorfile>
      <anchor>aad9e2e37b99c599bda845795dbebe07f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="variable">
      <type>std::function&lt; void(juce::String)&gt;</type>
      <name>onUpdate</name>
      <anchorfile>classUpdateChecker.html</anchorfile>
      <anchor>a2e082ff0c64af56dea0d00fbfd939e63</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ValueSmoother</name>
    <filename>classValueSmoother.html</filename>
    <templarg></templarg>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>a4c667c173bbe3cdc3cafa748464382e7</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setTime</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>a2fdfa7c891839b964c41d6d5660f9b0d</anchor>
      <arglist>(double t)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isSmoothing</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>aab80e3fc29490fd570be2339494f5c7f</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reset</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>a7c0b7e542ba5484825a54ea69612b8a7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getCurrentValue</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>a204256b57b1340450311edbf79386c27</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>T *</type>
      <name>getValuePtr</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>a0fd46737155bc081c73db8beacb16dd3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValue</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>a59b91b3e66200968b50d063c7ffe49d4</anchor>
      <arglist>(T v)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>snapToValue</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>ae961540da25f03bde80e1251ae9160ef</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>a98102883ab75aadce5dde661253fe0b2</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateValue</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>a581f8cc773e4cc2bac2da53e6edfec85</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getNextValue</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>ae344ea65895d160583cb8ad824521ced</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setValueUnsmoothed</name>
      <anchorfile>classValueSmoother.html</anchorfile>
      <anchor>ac361c441cfa04dd63ef694eed574adc0</anchor>
      <arglist>(T v)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>ValueTreeObject</name>
    <filename>classValueTreeObject.html</filename>
    <member kind="function">
      <type></type>
      <name>ValueTreeObject</name>
      <anchorfile>classValueTreeObject.html</anchorfile>
      <anchor>a50b6f4eead4ccc3a50257763f178eb47</anchor>
      <arglist>(const juce::ValueTree &amp;state)</arglist>
    </member>
    <member kind="function">
      <type>juce::ValueTree &amp;</type>
      <name>getState</name>
      <anchorfile>classValueTreeObject.html</anchorfile>
      <anchor>a380c3e637b87625deaaca5ee36e5834c</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const juce::OwnedArray&lt; ValueTreeObject &gt; &amp;</type>
      <name>getChildren</name>
      <anchorfile>classValueTreeObject.html</anchorfile>
      <anchor>a4ba83fcf6513054e49aa008e0ed7ba63</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>TargetClass *</type>
      <name>findParentOfType</name>
      <anchorfile>classValueTreeObject.html</anchorfile>
      <anchor>a65139d476b9d199ba0f29cffe89ca67c</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>juce::Array&lt; TargetClass * &gt;</type>
      <name>findChildrenOfClass</name>
      <anchorfile>classValueTreeObject.html</anchorfile>
      <anchor>ac5938920794ba2e6acceb5e85c820efa</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>countChildrenOfClass</name>
      <anchorfile>classValueTreeObject.html</anchorfile>
      <anchor>a0573fbf9bf2255241747c8ebc59325b1</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>TargetClass *</type>
      <name>findChildOfClass</name>
      <anchorfile>classValueTreeObject.html</anchorfile>
      <anchor>a872c180f78dd75884192baba68011ee6</anchor>
      <arglist>(int idx) const</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static std::function&lt; ValueTreeObject *(const juce::Identifier &amp;, const juce::ValueTree &amp;)&gt;</type>
      <name>factory</name>
      <anchorfile>classValueTreeObject.html</anchorfile>
      <anchor>a50b525ebb88fd07644ddbb78bb6ce11f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>VoicedStereoOscillator</name>
    <filename>classVoicedStereoOscillator.html</filename>
    <templarg></templarg>
    <class kind="struct">VoicedStereoOscillator::Params</class>
    <member kind="function">
      <type></type>
      <name>VoicedStereoOscillator</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>ac5443c70b9574352f495f46168e7f012</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>ac54d99a7902680164cfefb1f0623625f</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a319c65ef9916777690eb6cf3a7a117e9</anchor>
      <arglist>(float phase=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a639c3d573726cb847f78d016bcddd1c6</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processAdding</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a4d3e798a36e14f0ebd42b9ba0cc1c097</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::OwnedArray&lt; O &gt;</type>
      <name>oscillators</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a861cbdfd3fb820808a01d11aac7708e4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>VoicedStereoOscillator&lt; StereoOscillator &gt;</name>
    <filename>classVoicedStereoOscillator.html</filename>
    <member kind="function">
      <type></type>
      <name>VoicedStereoOscillator</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>ac5443c70b9574352f495f46168e7f012</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>ac54d99a7902680164cfefb1f0623625f</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a319c65ef9916777690eb6cf3a7a117e9</anchor>
      <arglist>(float phase=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a639c3d573726cb847f78d016bcddd1c6</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processAdding</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a4d3e798a36e14f0ebd42b9ba0cc1c097</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::OwnedArray&lt; StereoOscillator &gt;</type>
      <name>oscillators</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a861cbdfd3fb820808a01d11aac7708e4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>VoicedStereoOscillator&lt; WTOscillator &gt;</name>
    <filename>classVoicedStereoOscillator.html</filename>
    <member kind="function">
      <type></type>
      <name>VoicedStereoOscillator</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>ac5443c70b9574352f495f46168e7f012</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>ac54d99a7902680164cfefb1f0623625f</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a319c65ef9916777690eb6cf3a7a117e9</anchor>
      <arglist>(float phase=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a639c3d573726cb847f78d016bcddd1c6</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processAdding</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a4d3e798a36e14f0ebd42b9ba0cc1c097</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>juce::OwnedArray&lt; WTOscillator &gt;</type>
      <name>oscillators</name>
      <anchorfile>classVoicedStereoOscillator.html</anchorfile>
      <anchor>a861cbdfd3fb820808a01d11aac7708e4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>WaveformComponent</name>
    <filename>classWaveformComponent.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ColourIds</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a45b49a62cd0812131e81de9e1715fd01</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a45b49a62cd0812131e81de9e1715fd01a11782bc8650b22ee24fa433d8d973b05</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a45b49a62cd0812131e81de9e1715fd01ac7b94091fb6ee7e449758a8eee28014d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>traceColourId</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a45b49a62cd0812131e81de9e1715fd01ae41bbfdc8560f893683785e89ef15dc9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>envelopeColourId</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a45b49a62cd0812131e81de9e1715fd01a81751e2adb3bf06fef083f5c69fb1561</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a45b49a62cd0812131e81de9e1715fd01a11782bc8650b22ee24fa433d8d973b05</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a45b49a62cd0812131e81de9e1715fd01ac7b94091fb6ee7e449758a8eee28014d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>traceColourId</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a45b49a62cd0812131e81de9e1715fd01ae41bbfdc8560f893683785e89ef15dc9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>envelopeColourId</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a45b49a62cd0812131e81de9e1715fd01a81751e2adb3bf06fef083f5c69fb1561</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>WaveformComponent</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a5ea719fe0ab7464f7411fb5ec4eb5d4b</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~WaveformComponent</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>aa76bc8cf5e62496dd762857730c66768</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setHorizontalZoom</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a50f800af55a80afb60ed4fa7fdcdf54c</anchor>
      <arglist>(float zoom)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setHorizontalOffset</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a6f14e12cd54d059e1a8f21ff3dd16af8</anchor>
      <arglist>(float offset)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setBuffer</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>aaf911782a205ce4ea8b286fbbd05143f</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a76a7df6d1091e40e2f2bcb55eea3eaca</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resized</name>
      <anchorfile>classWaveformComponent.html</anchorfile>
      <anchor>a7cea6140d1de8b9a4d5f6c1f5a912aab</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>WEBPImageFormat</name>
    <filename>classWEBPImageFormat.html</filename>
    <member kind="function">
      <type>juce::String</type>
      <name>getFormatName</name>
      <anchorfile>classWEBPImageFormat.html</anchorfile>
      <anchor>a512523a82459ad3a42a50c7ebd176d2b</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>canUnderstand</name>
      <anchorfile>classWEBPImageFormat.html</anchorfile>
      <anchor>a1625a869b3485ebde4cbca83ee5076a7</anchor>
      <arglist>(juce::InputStream &amp;input) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>usesFileExtension</name>
      <anchorfile>classWEBPImageFormat.html</anchorfile>
      <anchor>a1d875430f06a78fe547812ff5556bbc0</anchor>
      <arglist>(const juce::File &amp;possibleFile) override</arglist>
    </member>
    <member kind="function">
      <type>juce::Image</type>
      <name>decodeImage</name>
      <anchorfile>classWEBPImageFormat.html</anchorfile>
      <anchor>a9ec17db624e1ed9512371a55b1ac52d9</anchor>
      <arglist>(juce::InputStream &amp;input) override</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>writeImageToStream</name>
      <anchorfile>classWEBPImageFormat.html</anchorfile>
      <anchor>a7323d308a2ed305d577d7630cfafc0b8</anchor>
      <arglist>(const juce::Image &amp;sourceImage, juce::OutputStream &amp;destStream) override</arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>quality</name>
      <anchorfile>classWEBPImageFormat.html</anchorfile>
      <anchor>a346bc04bdd1ff9881380f0c906cf86d4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>loseless</name>
      <anchorfile>classWEBPImageFormat.html</anchorfile>
      <anchor>a603e71ad278603009686b3290f8abcc1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>WebSocket</name>
    <filename>classWebSocket.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ReadyStateValues</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a452bcfacd2ce0567751e44f891dd1489</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CLOSING</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a452bcfacd2ce0567751e44f891dd1489a33e319b209babd422a32b44460e161f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CLOSED</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a452bcfacd2ce0567751e44f891dd1489adc1345c2f003ed355a242dd7079e674e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OPEN</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a452bcfacd2ce0567751e44f891dd1489ac928a123dad06890355c7b4197053358</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CLOSING</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a452bcfacd2ce0567751e44f891dd1489a33e319b209babd422a32b44460e161f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CLOSED</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a452bcfacd2ce0567751e44f891dd1489adc1345c2f003ed355a242dd7079e674e</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>OPEN</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a452bcfacd2ce0567751e44f891dd1489ac928a123dad06890355c7b4197053358</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~WebSocket</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a48afbe9b82fde4479913a1b931444729</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>readIncoming</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a2aef88773975bba1375290597806bc39</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>poll</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a785d925bd20d28b752313cc02991186e</anchor>
      <arglist>(int timeout=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>send</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>aa486d4abbd2faed495606e734c621093</anchor>
      <arglist>(const juce::String &amp;message)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sendBinary</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a3dcc5cb0d29d415f34a824aeab2e7e8c</anchor>
      <arglist>(const juce::String &amp;message)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sendBinary</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>aee695e7771f2d8c7d5bea250d5325abe</anchor>
      <arglist>(const juce::MemoryBlock &amp;message)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sendPing</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>ada4b76033e5fa73b3b76dd0a5b32de03</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>close</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>abef10acf60b4d5b87399e6f9c5ad808d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>ReadyStateValues</type>
      <name>getReadyState</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>ab64f7a28ecefa78a3143d499341ca585</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dispatch</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a757b2ded35954d331ceef6932ab331d6</anchor>
      <arglist>(std::function&lt; void(const juce::MemoryBlock &amp;message, bool isBinary)&gt; callback)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static WebSocket *</type>
      <name>fromURL</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a772a85561373a9548bd00ed7a7473c58</anchor>
      <arglist>(const juce::String &amp;url, const juce::String &amp;origin={})</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static WebSocket *</type>
      <name>fromURLNoMask</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>af03b816bb7e26fccad0d1b4bf9b0b5b3</anchor>
      <arglist>(const juce::String &amp;url, const juce::String &amp;origin={})</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static WebSocket *</type>
      <name>fromURL</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a7e2404176abec3d499adcd92fcc6bc42</anchor>
      <arglist>(const juce::String &amp;url, bool useMask, const juce::String &amp;origin)</arglist>
    </member>
    <member kind="variable">
      <type>ReadyStateValues</type>
      <name>readyStateValues</name>
      <anchorfile>classWebSocket.html</anchorfile>
      <anchor>a752b8c43bb821039435c92823261bd6d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>WetDryMix</name>
    <filename>classWetDryMix.html</filename>
    <member kind="function">
      <type></type>
      <name>WetDryMix</name>
      <anchorfile>classWetDryMix.html</anchorfile>
      <anchor>a31c06ff0212820a3b2b93571ef54127c</anchor>
      <arglist>(float mix)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMix</name>
      <anchorfile>classWetDryMix.html</anchorfile>
      <anchor>a624aa422b18039b0f497c88f336e42dc</anchor>
      <arglist>(float mix)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>mixToGain</name>
      <anchorfile>classWetDryMix.html</anchorfile>
      <anchor>a6a642b5b4d79eb59f61ccfd70c67ae77</anchor>
      <arglist>(float mix)</arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>wetGain</name>
      <anchorfile>classWetDryMix.html</anchorfile>
      <anchor>ab3f2edb9437368c6f3d87e51a64a3f2d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>float</type>
      <name>dryGain</name>
      <anchorfile>classWetDryMix.html</anchorfile>
      <anchor>a56424d33affe35b8fda304af339ed787</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>WSHeaderType</name>
    <filename>structWSHeaderType.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Opcode</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CONTINUATION</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a281dd9cfd2da201efff9e0e33950c593</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TEXT_FRAME</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a44b257e3d1f0e6e8ee821d8ce460cea2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BINARY_FRAME</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a1ed79e07a2c73eaa0cc27ca3348dc8cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CLOSE</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a595b1cfbd1c8acb916bf1bb6dcb8de9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>PING</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a5b3e1bd1c7c96b5ba780ee84ddf37117</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>PONG</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a90905d922e4dcd25b14950537e85e311</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CONTINUATION</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a281dd9cfd2da201efff9e0e33950c593</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>TEXT_FRAME</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a44b257e3d1f0e6e8ee821d8ce460cea2</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>BINARY_FRAME</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a1ed79e07a2c73eaa0cc27ca3348dc8cc</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>CLOSE</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a595b1cfbd1c8acb916bf1bb6dcb8de9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>PING</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a5b3e1bd1c7c96b5ba780ee84ddf37117</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>PONG</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac1906191ac337fdbecbe643184fbd3b3a90905d922e4dcd25b14950537e85e311</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>unsigned</type>
      <name>header_size</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ae61a7f14521c6529a754c0b414c2e6e5</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>fin</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>ac7dd766ad7ca4f9b86b69d895ed504d3</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>bool</type>
      <name>mask</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>aa436b6fcc692f760eb348b921409d8f4</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Opcode</type>
      <name>opcode</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>afbb7bd8db39bf2a68e8055e7d2f0c4db</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>N0</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>a6c084039602e19593f16e198194258ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint64_t</type>
      <name>N</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>a280de6932d75f1d0b3f5687f839d4e92</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>uint8_t</type>
      <name>masking_key</name>
      <anchorfile>structWSHeaderType.html</anchorfile>
      <anchor>aaf830be2305c270185628430337c50b1</anchor>
      <arglist>[4]</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>WTOscillator</name>
    <filename>classWTOscillator.html</filename>
    <class kind="struct">WTOscillator::Params</class>
    <member kind="function">
      <type></type>
      <name>WTOscillator</name>
      <anchorfile>classWTOscillator.html</anchorfile>
      <anchor>a16fdc8afe3d72d02755d37bb13aa3dad</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSampleRate</name>
      <anchorfile>classWTOscillator.html</anchorfile>
      <anchor>ac982050906c9ca3bb2097fdc85f3c444</anchor>
      <arglist>(double sr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>noteOn</name>
      <anchorfile>classWTOscillator.html</anchorfile>
      <anchor>aa5b7bfe2f4f6f4616af869e7ff6977b3</anchor>
      <arglist>(float p=-1)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classWTOscillator.html</anchorfile>
      <anchor>a7ec6890f0ace8c49239413a93e4896e6</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>process</name>
      <anchorfile>classWTOscillator.html</anchorfile>
      <anchor>af7351513ecfdd36c303b15599a2a4de9</anchor>
      <arglist>(float noteL, float noteR, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processAdding</name>
      <anchorfile>classWTOscillator.html</anchorfile>
      <anchor>aaa547794aafc04f582782983988b2651</anchor>
      <arglist>(float note, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>processAdding</name>
      <anchorfile>classWTOscillator.html</anchorfile>
      <anchor>a98722a2a585c5d4919ee0f794e8ce96c</anchor>
      <arglist>(float noteL, float noteR, const Params &amp;params, juce::AudioSampleBuffer &amp;buffer)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setWavetable</name>
      <anchorfile>classWTOscillator.html</anchorfile>
      <anchor>aa9f27ee415927875063cb2203ec3f97c</anchor>
      <arglist>(juce::OwnedArray&lt; BandLimitedLookupTable &gt; &amp;table)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>WTVoicedStereoOscillator</name>
    <filename>classWTVoicedStereoOscillator.html</filename>
    <base>VoicedStereoOscillator&lt; WTOscillator &gt;</base>
    <member kind="function">
      <type></type>
      <name>WTVoicedStereoOscillator</name>
      <anchorfile>classWTVoicedStereoOscillator.html</anchorfile>
      <anchor>a8db7ce7f1c1f034e9f3d38119ed1325d</anchor>
      <arglist>(int maxVoices=8)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setWavetable</name>
      <anchorfile>classWTVoicedStereoOscillator.html</anchorfile>
      <anchor>a19bd8496f404f6c76df2776ac282f44f</anchor>
      <arglist>(juce::OwnedArray&lt; BandLimitedLookupTable &gt; &amp;table)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>XmpMetadata</name>
    <filename>classXmpMetadata.html</filename>
    <base>ImageMetadata</base>
    <member kind="function">
      <type></type>
      <name>XmpMetadata</name>
      <anchorfile>classXmpMetadata.html</anchorfile>
      <anchor>abfde5b535622830bfc3ed00a446e2285</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~XmpMetadata</name>
      <anchorfile>classXmpMetadata.html</anchorfile>
      <anchor>a2e38bf250cc3ebbb60e290ce041017e7</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>juce::StringPairArray</type>
      <name>getAllMetadata</name>
      <anchorfile>classXmpMetadata.html</anchorfile>
      <anchor>aa9dcfa26976a1e76877edfe03b1c2602</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static XmpMetadata *</type>
      <name>createFromJpg</name>
      <anchorfile>classXmpMetadata.html</anchorfile>
      <anchor>ab838b43e78d3aa5ae6f4a8a121737d88</anchor>
      <arglist>(const juce::uint8 *data, int sz)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static XmpMetadata *</type>
      <name>createFromPng</name>
      <anchorfile>classXmpMetadata.html</anchorfile>
      <anchor>af39cbad415533fe7acf5ab2963ab261e</anchor>
      <arglist>(const char *data, int sz)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>XYScope</name>
    <filename>classXYScope.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>ColourIds</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>a8c9be5fbb6205a1a9920d2068ca13b22</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>a8c9be5fbb6205a1a9920d2068ca13b22af5d0d044e9ac695a2ee35d1365a93a47</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>a8c9be5fbb6205a1a9920d2068ca13b22aa920a9d6adc56714a5b62c9f25eb4c4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>traceColourId</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>a8c9be5fbb6205a1a9920d2068ca13b22ab74a667b5c450498ad536a857e77d35a</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>lineColourId</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>a8c9be5fbb6205a1a9920d2068ca13b22af5d0d044e9ac695a2ee35d1365a93a47</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>backgroundColourId</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>a8c9be5fbb6205a1a9920d2068ca13b22aa920a9d6adc56714a5b62c9f25eb4c4f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>traceColourId</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>a8c9be5fbb6205a1a9920d2068ca13b22ab74a667b5c450498ad536a857e77d35a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>XYScope</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>ac2e4be42faf159ede8fc6f8baaa2829c</anchor>
      <arglist>(AudioFifo &amp;)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~XYScope</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>a53af5ce97964c41b051e2396cd9a6bec</anchor>
      <arglist>() override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setNumSamplesPerPixel</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>ab0e51490e9997247e0df9d78f938fb2f</anchor>
      <arglist>(float newNumSamplesPerPixel)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setZoomFactor</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>af4c855839113e54dd65e51bf2b063ca8</anchor>
      <arglist>(float newZoomFactor)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>paint</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>abb9b5a73646c70d9e823a7a4676aa3fa</anchor>
      <arglist>(juce::Graphics &amp;g) override</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>timerCallback</name>
      <anchorfile>classXYScope.html</anchorfile>
      <anchor>a37d7c0b92a484c3c55dee8eb45fd2943</anchor>
      <arglist>() override</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>Lagrange</name>
    <filename>namespaceLagrange.html</filename>
    <member kind="function">
      <type>T</type>
      <name>interpolate</name>
      <anchorfile>namespaceLagrange.html</anchorfile>
      <anchor>aed0cc45b276f9ed337143c4587dc1122</anchor>
      <arglist>(const juce::Array&lt; Point&lt; T &gt;&gt; &amp;points, T x)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>interpolate</name>
      <anchorfile>namespaceLagrange.html</anchorfile>
      <anchor>ab812f4c6079c4c45b43e56239c3634d4</anchor>
      <arglist>(T xArr[], T yArr[], int num, T x)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>Resources</name>
    <filename>namespaceResources.html</filename>
    <member kind="variable">
      <type>const char *</type>
      <name>BarlowRegular_ttf</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>abbddb191fa7624c2d2127ce3b537b898</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>BarlowRegular_ttfSize</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>ab6e0bb1c70ec3ed211118f968880cd2f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>BarlowThin_ttf</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>ae4ad6d277ad33de26e02e1572510d8dc</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>BarlowThin_ttfSize</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>ae17a5aae2e3106082be4c8b5d3e61bf9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const char *</type>
      <name>InterRegular_otf</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>a821eb0a806b852016c53ab6151f00d3d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>const int</type>
      <name>InterRegular_otfSize</name>
      <anchorfile>namespaceResources.html</anchorfile>
      <anchor>af4f0aa394cb30c14c4306162f0d5e6d4</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>gin_gui-components</name>
    <title>components</title>
    <filename>group__gin__gui-components.html</filename>
    <member kind="function">
      <type>void</type>
      <name>addAndMakeVisible</name>
      <anchorfile>group__gin__gui-components.html</anchorfile>
      <anchor>ga66410efb088d24a02a86b4c6191e2866</anchor>
      <arglist>(juce::Component &amp;parent, juce::Array&lt; juce::Component * &gt; children)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addAndMakeVisible</name>
      <anchorfile>group__gin__gui-components.html</anchorfile>
      <anchor>gaa5dca7c16023c4ec901f0db44aaf89de</anchor>
      <arglist>(juce::Component &amp;parent, std::initializer_list&lt; juce::Component * &gt; children)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>gin_dsp-components</name>
    <title>components</title>
    <filename>group__gin__dsp-components.html</filename>
  </compound>
  <compound kind="group">
    <name>gin_plugin-components</name>
    <title>components</title>
    <filename>group__gin__plugin-components.html</filename>
    <member kind="function">
      <type>void</type>
      <name>gradientRect</name>
      <anchorfile>group__gin__plugin-components.html</anchorfile>
      <anchor>gae617c6797d9252a57e49dc7bef2f487b</anchor>
      <arglist>(juce::Graphics &amp;g, juce::Rectangle&lt; int &gt; rc, juce::Colour c1, juce::Colour c2)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>gin_dsp-dsp</name>
    <title>dsp</title>
    <filename>group__gin__dsp-dsp.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>Wave</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gadb931b2f752b7879df254a7fd9d17e90</anchor>
      <arglist></arglist>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a0e82533c37fced763be34a5c21d720c9">silence</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a925212d1624625aed9491413b4b7be05">sine</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90acef44b46f16ae8ecf664df4266ffdbf9">triangle</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90acf87f9eb2bfacc5a0362031eba539290">sawUp</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a3593df94728cd88f078bbc6c41df9876">sawDown</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a02a8dc4cf01fed584c6423f577c0b0d7">pulse</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a2fc01ec765ec0cb3dcc559126de20b30">square</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90aaaddc3454ccbefbb2d8d8461f8f7f481">noise</enumvalue>
      <enumvalue file="group__gin__dsp-dsp.html" anchor="ggadb931b2f752b7879df254a7fd9d17e90a6f0316b89c2034b8fabe6d3e48f15bbf">wavetable</enumvalue>
    </member>
    <member kind="function">
      <type>juce::Value</type>
      <name>findValue</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga642bc58b032577b139e71665cc1817dc</anchor>
      <arglist>(juce::ValueTree &amp;state, juce::Identifier name, juce::var value)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getMidiNoteInHertz</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga0521f87a41bf33dc06cc4b6889860401</anchor>
      <arglist>(const T noteNumber, const T frequencyOfA=(T) 440.0)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>getMidiNoteFromHertz</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gaba92011bb25f7416e8752e34fbec9c6f</anchor>
      <arglist>(const T freq, const T frequencyOfA=(T) 440.0)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>velocityToGain</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gaeda2ea585490695813285661264a92b9</anchor>
      <arglist>(float velocity, float sensitivity=1.0f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGain</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gaea524d06c0f9a97e215fafc7e997cc0b</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, juce::LinearSmoothedValue&lt; float &gt; &amp;gain)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGain</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga23129d8a0f12afa042d9788afa21b90f</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, int channel, juce::LinearSmoothedValue&lt; float &gt; &amp;gain)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clip</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga80d41603f74938619f0af6e8007f5944</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;buffer, float low=-1.0f, float high=1.0f)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>getMidiMessageType</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga6e16d794b5bc34b561ea9107adbd69ce</anchor>
      <arglist>(const juce::MidiMessage &amp;msg)</arglist>
    </member>
    <member kind="function">
      <type>juce::AudioSampleBuffer</type>
      <name>sliceBuffer</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga1f707ada89f48bb4f68c4890ba90c27b</anchor>
      <arglist>(juce::AudioSampleBuffer &amp;input, int start, int length)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>lerp</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga48b0dea68bc2af522095b90b5e9acc13</anchor>
      <arglist>(T a, T b, T t)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>sign</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga1764c1268bc41fe5590a1287c071b27e</anchor>
      <arglist>(T x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sine</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gac81d5fa67210b71931d9191c641871b2</anchor>
      <arglist>(double phase, double unused1=0, double unused2=0)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>triangle</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga83fe6fa8512b1a676092cf10f9c79e7e</anchor>
      <arglist>(double phase, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sawUp</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga54fd9fc5216539143983df52fcfd38be</anchor>
      <arglist>(double phase, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sawDown</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gaf4e429b20b8c3d03b8527af2f1d7a14f</anchor>
      <arglist>(double phase, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>pulse</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga5d70ff16cbda6f79543b487e6831052a</anchor>
      <arglist>(double phase, double pw, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>squareWave</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gad6edce63379d418ac8ceb7f57a97bfc6</anchor>
      <arglist>(double phase, double freq, double sampleRate)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>noise</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga144d0a5afb304f562a311e0d62e77dc6</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>loadWavetables</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>ga8167e97c7e5fc9916a3423ca494b72b8</anchor>
      <arglist>(juce::OwnedArray&lt; BandLimitedLookupTable &gt; &amp;bllt, juce::AudioSampleBuffer &amp;buffer, double sampleRate, int tableSize)</arglist>
    </member>
    <member kind="variable">
      <type>constexpr float</type>
      <name>Q</name>
      <anchorfile>group__gin__dsp-dsp.html</anchorfile>
      <anchor>gab7b12a212b5068cd4b83c5ef0af70511</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>gin_graphics-formats</name>
    <title>formats</title>
    <filename>group__gin__graphics-formats.html</filename>
  </compound>
  <compound kind="group">
    <name>gin-geometry</name>
    <title>geometry</title>
    <filename>group__gin-geometry.html</filename>
    <member kind="function">
      <type>T</type>
      <name>square</name>
      <anchorfile>group__gin-geometry.html</anchorfile>
      <anchor>ga1c4cf92c230b2e375db8d832d07ec041</anchor>
      <arglist>(T in)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>solveLine</name>
      <anchorfile>group__gin-geometry.html</anchorfile>
      <anchor>ga97bafa72f9f8ecdba9b839ecdb3db767</anchor>
      <arglist>(T x1, T y1, T x2, T y2, T &amp;m, T &amp;b)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>gin</name>
    <title>gin</title>
    <filename>group__gin.html</filename>
    <subgroup>gin-geometry</subgroup>
    <subgroup>gin-utilities</subgroup>
  </compound>
  <compound kind="group">
    <name>gin_dsp</name>
    <title>gin_dsp</title>
    <filename>group__gin__dsp.html</filename>
    <subgroup>gin_dsp-components</subgroup>
    <subgroup>gin_dsp-dsp</subgroup>
  </compound>
  <compound kind="group">
    <name>gin_graphics</name>
    <title>gin_graphics</title>
    <filename>group__gin__graphics.html</filename>
    <subgroup>gin_graphics-formats</subgroup>
    <subgroup>gin_graphics-images</subgroup>
  </compound>
  <compound kind="group">
    <name>gin_gui</name>
    <title>gin_gui</title>
    <filename>group__gin__gui.html</filename>
    <subgroup>gin_gui-components</subgroup>
    <subgroup>gin_gui-images</subgroup>
    <subgroup>gin_gui-utilities</subgroup>
  </compound>
  <compound kind="group">
    <name>gin_metadata</name>
    <title>gin_metadata</title>
    <filename>group__gin__metadata.html</filename>
    <subgroup>gin_metadata-metadata</subgroup>
  </compound>
  <compound kind="group">
    <name>gin_network</name>
    <title>gin_network</title>
    <filename>group__gin__network.html</filename>
    <subgroup>gin_network-network</subgroup>
  </compound>
  <compound kind="group">
    <name>gin_plugin</name>
    <title>gin_plugin</title>
    <filename>group__gin__plugin.html</filename>
    <subgroup>gin_plugin-components</subgroup>
    <subgroup>gin_plugin-lookandfeel</subgroup>
    <subgroup>gin_plugin-plugin</subgroup>
    <subgroup>gin_plugin-resources</subgroup>
  </compound>
  <compound kind="group">
    <name>gin_graphics-images</name>
    <title>images</title>
    <filename>group__gin__graphics-images.html</filename>
    <member kind="enumeration">
      <type></type>
      <name>BlendMode</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga30323fa90e24ca484998b9fba6c40e85</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Normal</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a5ecbcf0afce98b042f35ec71ba03fa4b</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Lighten</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a0c4863f7d789ab70b21073bd94c9169d</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Darken</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a95ea73d8fed63115639319a67d596d49</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Multiply</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ade55cee401a6a53619b6ec469abf0810</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Average</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ab3c87ec2c47256239220b24e46acda7f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Add</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a0d592a4b562059bc283e00b3704865c9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Subtract</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85afa8a9720422ee254970b2ebe1435a917</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Difference</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a9cba343e00a6259dc83f7e4e7f447109</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Negation</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ab94b0efce58a77e1c8fa29e32536c728</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Screen</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a2609fadb77bb3d98c615b41ac79a1d31</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Exclusion</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ac200cda01c78d8dae2b11cf4ded47655</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Overlay</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a8b895d92539a718e6898ed6b053ce2ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>SoftLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a967749a14f4bb7ffc6383997d97a6163</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HardLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a7fc75582ae37ab60f5a1cc8e2acd7e24</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ColorDodge</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a71b5386d914bd4101f3d086d8ce72161</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>ColorBurn</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85adccc18afee7dc5a40147cab59118d583</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LinearDodge</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85afe7aab6a1580381724d2ebe724fbd065</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LinearBurn</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a4c41b912f1e9eca71a1a36fc8a1172d9</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>LinearLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ae158a552ecb4c88077b7537e93a8df86</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>VividLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a87be187db30f3efdb79dde93f63e0430</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>PinLight</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a513bf9b05af260d001632c4fe6d345d5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>HardMix</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a76ee1ad58cc676f7994416354a4d7b59</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Reflect</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a8898819fa0ebd77a64e192d47e7be553</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Glow</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85ac7adb89e56ad4a2daef183d318110442</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>Phoenix</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gga30323fa90e24ca484998b9fba6c40e85a85c155bffb4118332192060f1a6db976</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyVignette</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga1023b87ad7bb7ca1e039ded54397ef90</anchor>
      <arglist>(juce::Image &amp;img, float amount, float radius, float falloff, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applySepia</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gaccaa511049769192c15aa13e0c2a40d1</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGreyScale</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gadb285c05d7e81fbffe8b2ab5705b0783</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applySoften</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga4cc9c0919994beeac34d088beb1991e2</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applySharpen</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gac0b5514a6c74e31bd4e7a26898e45228</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGamma</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga2b20d743c3475fd86d18a8f2cb427b47</anchor>
      <arglist>(juce::Image &amp;img, float gamma, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyInvert</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga6453f04ab3d2a6a345cd6b7f70711c04</anchor>
      <arglist>(juce::Image &amp;img, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyContrast</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga50c77bf67a02fbc002d1136fd64f2031</anchor>
      <arglist>(juce::Image &amp;img, float contrast, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyBrightnessContrast</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gab3825059a9e75bfc8e78cf0106ddf78e</anchor>
      <arglist>(juce::Image &amp;img, float brightness, float contrast, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyHueSaturationLightness</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga25205b0926bfca178115404c6d9f45d9</anchor>
      <arglist>(juce::Image &amp;img, float hue, float saturation, float lightness, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyStackBlur</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga5298c1220caef0f1d6d918c9ec15243d</anchor>
      <arglist>(juce::Image &amp;img, int radius)</arglist>
    </member>
    <member kind="function">
      <type>juce::Image</type>
      <name>applyResize</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga8a045522d5a03954f0d4fe2cbc8082a4</anchor>
      <arglist>(const juce::Image &amp;img, int width, int height)</arglist>
    </member>
    <member kind="function">
      <type>juce::Image</type>
      <name>applyResize</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga99654c39e56b66a36c75cbbff9e137b4</anchor>
      <arglist>(const juce::Image &amp;img, float factor)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGradientMap</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga2e91627e4ee0f9e1aeca78ca387a8c51</anchor>
      <arglist>(juce::Image &amp;img, const juce::ColourGradient &amp;gradient, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyGradientMap</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga48f6d1c24d18edd26e2d9da48e47dcf7</anchor>
      <arglist>(juce::Image &amp;img, const juce::Colour c1, const juce::Colour c2, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyColour</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>gae6d6ea52612fb4f6474b05a1063604db</anchor>
      <arglist>(juce::Image &amp;img, juce::Colour c, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyBlend</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga6e16e6b71a97615070d0f5f6829f2b04</anchor>
      <arglist>(juce::Image &amp;dst, const juce::Image &amp;src, BlendMode mode, float alpha=1.0f, juce::Point&lt; int &gt; position={}, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>applyBlend</name>
      <anchorfile>group__gin__graphics-images.html</anchorfile>
      <anchor>ga590207f6293f03a5a4e4e2a1d4f3db15</anchor>
      <arglist>(juce::Image &amp;dst, BlendMode mode, juce::Colour c, juce::ThreadPool *threadPool=nullptr)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>gin_gui-images</name>
    <title>images</title>
    <filename>group__gin__gui-images.html</filename>
    <member kind="function">
      <type>juce::Image</type>
      <name>rasterizeSVG</name>
      <anchorfile>group__gin__gui-images.html</anchorfile>
      <anchor>gaad1e06e3c606f89456dac559e0d417a7</anchor>
      <arglist>(juce::String svgText, int w, int h)</arglist>
    </member>
    <member kind="function">
      <type>juce::Path</type>
      <name>parseSVGPath</name>
      <anchorfile>group__gin__gui-images.html</anchorfile>
      <anchor>ga781250a0ec9422dc8c7365cf6b9935ee</anchor>
      <arglist>(const juce::String &amp;txt)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>gin_plugin-lookandfeel</name>
    <title>lookandfeel</title>
    <filename>group__gin__plugin-lookandfeel.html</filename>
  </compound>
  <compound kind="group">
    <name>gin_metadata-metadata</name>
    <title>metadata</title>
    <filename>group__gin__metadata-metadata.html</filename>
  </compound>
  <compound kind="group">
    <name>gin_network-network</name>
    <title>network</title>
    <filename>group__gin__network-network.html</filename>
  </compound>
  <compound kind="group">
    <name>gin_plugin-plugin</name>
    <title>plugin</title>
    <filename>group__gin__plugin-plugin.html</filename>
    <member kind="define">
      <type>#define</type>
      <name>ProcessorBaseClass</name>
      <anchorfile>group__gin__plugin-plugin.html</anchorfile>
      <anchor>ga9ccf6c679c1fdf1c1414992a17202f5a</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>getValue</name>
      <anchorfile>group__gin__plugin-plugin.html</anchorfile>
      <anchor>gab084f6f52a2bf9f5141a63554b6d22bb</anchor>
      <arglist>(gin::Parameter *p)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>gin_plugin-resources</name>
    <title>resources</title>
    <filename>group__gin__plugin-resources.html</filename>
  </compound>
  <compound kind="group">
    <name>gin-utilities</name>
    <title>utilities</title>
    <filename>group__gin-utilities.html</filename>
    <member kind="function">
      <type>T</type>
      <name>easeLinear</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga7646e77db052bff3178b73c722206e52</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuadraticIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga033a11422391bbad3d495e863729565d</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuadraticOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga70b838d26aa2fa360932a8716b6ff18f</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuadraticInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gabfebd9814de3e919edb3cd7516b9050d</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCubicIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gadd816be517ee34d5d5d6c03f58e6d615</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCubicOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga0a5b2a4c5ec14e5dd08e5c97ae5c7593</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCubicInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaaee23821c48a7585e68041e964fd8b2e</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuarticIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gae7d3fcf4936075991a42ec05b1ab1c53</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuarticOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga0141f33c3cea06bd2b792fd1aec21987</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuarticInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga8a97392f7cf1947e253594c713c73969</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuinticIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga25b4d43e01068724a839dc92f333f24c</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuinticOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga499817d290b5d1d7c0b83c5e0c2f80ac</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeQuinticInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gac2867f6fb639343e139634bf45c4d106</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeSineIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga5ff43623fe37daf28fc4dea493beb3a4</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeSineOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gab7eda190492ca09c29cc0777ac6451cf</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeSineInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga12bf8c86b1b6627bf353ff742d7d711e</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCircularIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga101b272b8b327afdd7b73be2da6d0c6b</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCircularOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga1b69a1d31e0fdc30a209976965266970</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeCircularInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga59090fad80e0b20c15074345995c21dd</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeExponentialIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaf1d0fd36995e8a9b9401a5966afeccb3</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeExponentialOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gac84889aa5e141b6fb2185e1877be552a</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeExponentialInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga66172ec142beaa1682ec8800a2e968bf</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeElasticIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaa583a283fcdb33f6658e7d5f45f9ea86</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeElasticOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga5d166785781b41c81e743e689b7b3314</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeElasticInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gabb5639d85f795ff77e4b553ab9dfcf1c</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBackIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga2fe38f966343690da01444f64772cd7a</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBackOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaa8cce7b392a0034c4c947b07a94dab45</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBackInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga0546a40d7cf9b955aa1ea464ea2e9129</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBounceIn</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga053d2907e791ee49187e6dfee114952b</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBounceOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gab198cd18c0de013a626ab39fd09917ce</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>easeBounceInOut</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga16564d349e6fe228db0c8b22df5d7bb4</anchor>
      <arglist>(T p)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>overwriteWithText</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga1aac7c94939802d4c2c08d600feefa23</anchor>
      <arglist>(const juce::File &amp;f, const juce::String &amp;text, bool asUnicode=false, bool writeUnicodeHeaderBytes=false, const char *lineEndings=nullptr)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>overwriteWithData</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga6e2dbf5f19342f51ea56d96cb5e9e842</anchor>
      <arglist>(const juce::File &amp;f, const juce::MemoryBlock &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>overwriteWithData</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gae1a4b4ab413ade0e4c90ad3d26ce854b</anchor>
      <arglist>(const juce::File &amp;f, const void *data, size_t size)</arglist>
    </member>
    <member kind="function">
      <type>juce::var</type>
      <name>parsePlist</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gac2aec1c96e46e294133bec5727ad2e98</anchor>
      <arglist>(const juce::File &amp;f)</arglist>
    </member>
    <member kind="function">
      <type>juce::var</type>
      <name>parsePlist</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gab0ff62a7c0a17d71004b40eb5a56f6a4</anchor>
      <arglist>(const juce::String &amp;s)</arglist>
    </member>
    <member kind="function">
      <type>juce::var</type>
      <name>parsePlist</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga745860e41e38bb0e8b5e52cd7655628a</anchor>
      <arglist>(const juce::XmlElement &amp;f)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>callInBackground</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gafe89b1694e591a29d7c325e41b21543f</anchor>
      <arglist>(std::function&lt; void(void)&gt; function)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>multiThreadedFor</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga383aa0fdd6752ac48ba3e087006730fa</anchor>
      <arglist>(T start, T end, T interval, juce::ThreadPool *threadPool, std::function&lt; void(T idx)&gt; callback)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>compareAndReset</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga1ebc11c7a7ef780026ec5a519893895e</anchor>
      <arglist>(bool &amp;flag)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>calculateRMS</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga458ba95824635202e48f00c71ced463d</anchor>
      <arglist>(const float *values, int n)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>calculateMedian</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga823d49b3f21b97485eed223caec940c6</anchor>
      <arglist>(const float *values, int n)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>shuffleArray</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaac60b7f594994fac0cb43d317f794449</anchor>
      <arglist>(juce::Random &amp;r, T array)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>almostEqual</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga06700101fa042b926bad6b16738aff53</anchor>
      <arglist>(T a, T b, T precision=T(0.00001))</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>versionStringToInt</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga9861500c17d0717e211dc67c84ccc261</anchor>
      <arglist>(const juce::String &amp;versionString)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>delayedLambda</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaa8e8a032e70b00eb8b977cadd0fb09a8</anchor>
      <arglist>(std::function&lt; void()&gt; callback, int delayMS)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>formatNumber</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gaebf910d77d287573a8f98f51c67f8d48</anchor>
      <arglist>(T v)</arglist>
    </member>
    <member kind="function">
      <type>juce::String</type>
      <name>valueTreeToJSON</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>gac8809f5a07d326dc76f18d09bd99db9d</anchor>
      <arglist>(const juce::ValueTree &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>juce::ValueTree</type>
      <name>valueTreeFromJSON</name>
      <anchorfile>group__gin-utilities.html</anchorfile>
      <anchor>ga88f482bf81fb3e423fa1e3926dfaa9a6</anchor>
      <arglist>(const juce::String &amp;jsonText)</arglist>
    </member>
  </compound>
  <compound kind="group">
    <name>gin_gui-utilities</name>
    <title>utilities</title>
    <filename>group__gin__gui-utilities.html</filename>
    <member kind="function">
      <type>void</type>
      <name>callOnMainThreadBlocking</name>
      <anchorfile>group__gin__gui-utilities.html</anchorfile>
      <anchor>ga9fefd2dceb6369dbeac0b093b14c72fa</anchor>
      <arglist>(std::function&lt; void()&gt; func)</arglist>
    </member>
    <member kind="function">
      <type>juce::Colour</type>
      <name>goldenRatioColor</name>
      <anchorfile>group__gin__gui-utilities.html</anchorfile>
      <anchor>ga345f5b8f3ffdd680b206fe42f25be481</anchor>
      <arglist>(int idx)</arglist>
    </member>
  </compound>
</tagfile>
