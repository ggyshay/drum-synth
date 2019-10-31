class MasterChannel {
  public:
    MasterChannel() {
      patch0 = new AudioConnection(output1, _output);
    }
    AudioMixer4       output1;
    AudioMixer4       output2;
    AudioAmplifier _output;
  private:
    AudioConnection *patch0;
}
