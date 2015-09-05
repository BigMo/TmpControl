struct Button
{
	unsigned short m_iPin;
	bool lastState, currentState;
	
	void SetUp(unsigned short p_iPin)
	{
		this->m_iPin = p_iPin;
		this->lastState = false;
		this->currentState = false;
		
		digitalWrite(this->m_iPin, HIGH);		
	}
	
	void Update()
	{
		this->lastState = this->currentState;
		this->currentState = digitalRead(this->m_iPin) == LOW;
	}
	
	bool IsDown() { return this->currentState; }
	bool WentDown() { return !this->lastState && this->currentState; }
	bool WentUp() { return this->lastState && !this->currentState; }
	bool Changed() { return this->lastState != this->currentState; }
};